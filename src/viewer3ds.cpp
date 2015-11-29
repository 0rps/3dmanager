#include <math.h>

#include <QMouseEvent>
#include <QDebug>
#include <QContextMenuEvent>
#include <QTimerEvent>
#include <QFile>
#include <QBuffer>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QFile>

#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/light.h>
#include <lib3ds/mesh.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>
#include <lib3ds/material.h>
#include <lib3ds/camera.h>
#include <lib3ds/io.h>
#include <lib3ds/vector.h>

#include "wrapperqiodevice.h"
#include "camera.h"
#include "viewer3ds.h"


Viewer3ds::Viewer3ds(QWidget *parent) :
    View(parent),
    m_file(0),
    m_textureId(0),
    m_currentFrame(0)
{
    setAcceptDrops(true);
}

Viewer3ds::~Viewer3ds()
{
    clean();
}

void Viewer3ds::clean()
{
    makeCurrent();
    if(m_file)
        lib3ds_file_free(m_file);
    m_file = 0;

    camera()->setRotate( QVector3D(0.0, 0.0 ,0.0) );
    camera()->setScale( 1.0 );
    updateGL();
}

void Viewer3ds::cleanTexture()
{
    makeCurrent();
    deleteTexture(m_textureId);
    m_textureId = 0;
}

void Viewer3ds::contextMenuEvent(QContextMenuEvent *event)
{
    emit sgnNeedContextMenu( this, event->globalPos() );
}

bool Viewer3ds::loadTextureFile(const QString &_fileName)
{
    if(_fileName.isEmpty())
    {
        cleanTexture();
        return false;
    }

    QFile file(_fileName);
    return loadTextureFile(file);
}

bool Viewer3ds::loadFile(const QByteArray &_data)
{
    QByteArray p_data = _data;
    QBuffer pBuffer(&p_data);
    return loadFile(pBuffer);
}

bool Viewer3ds::loadTextureFile(const QByteArray &_data)
{
    QByteArray p_data = _data;
    QBuffer pBuffer(&p_data);
    return loadTextureFile(pBuffer);
}

bool Viewer3ds::loadTextureFile(QIODevice &_ioDevice)
{
    cleanTexture();

    if(!_ioDevice.open(QIODevice::ReadOnly))
        return false;

    QImage imgTexture;
    if(!imgTexture.loadFromData(_ioDevice.readAll()))
        return false;

    m_textureId = bindTexture ( imgTexture );
    qDebug() << " binding texture " << m_textureId;
    updateGL();
    return true;
}

bool Viewer3ds::loadFile(const QString &_fileName)
{
    if(_fileName.isEmpty())
    {
        clean();
        return false;
    }

    QFile file(_fileName);
    return loadFile(file);
}

bool Viewer3ds::loadFile(QIODevice &_ioDevice)
{
    clean();

    m_file = lib3ds::lib3ds_iodevice_load(_ioDevice);
    if(!m_file)
    {
        qWarning("Error : Unable to load file ");
        return false;
    }

    // No nodes?  Fabricate nodes to display all the meshes.
    if( !m_file->nodes )
        addDefaultNodes();

    lib3ds_file_eval(m_file, 1.0f);

    // No cameras in the file?  Add four
    if( !m_file->cameras  && !addDefaultCamera() )
    {
        qDebug() << "3dsplayer: Error: No Camera found.";
        clean();
        return  false;
    }

    lib3ds_file_eval(m_file, 0.0f);

    initCamera();
    initSceneLight();

    startAnimate();
    return true;
}

void Viewer3ds::drawScene()
{
    if(!m_file)
        return;

    if(m_textureId)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
    }

    for (Lib3dsNode* pNode = m_file->nodes; pNode != 0; pNode = pNode->next)
        renderNode(pNode);

    glDisable(GL_TEXTURE_2D);
}

void Viewer3ds::dragEnterEvent(QDragEnterEvent *_event)
{
    if (_event->mimeData()->hasUrls())
        _event->acceptProposedAction();
}

void Viewer3ds::dropEvent(QDropEvent *_event)
{
    const QMimeData*_data = _event->mimeData();
    if (_data->hasUrls())
    {
        QList<QUrl> p_urls = _data->urls();

        QString p_3ds;
        QString p_image;
        QString p_dirPath;

        foreach(const QUrl &_url, p_urls)
        {
            QString p_infoStr = _url.toString().remove("file:///");
            QFileInfo p_info(p_infoStr);
            if (p_info.isDir())
            {
                p_dirPath = p_infoStr;
            } else if (p_info.isFile())
            {
                if (p_info.fileName().contains(QRegExp("3ds$",Qt::CaseInsensitive)))
                {
                    p_3ds = p_info.absoluteFilePath();
                } else if (p_info.fileName().contains(QRegExp("(png|jpg|jpeg)$",Qt::CaseInsensitive)))
                {
                    p_image = p_info.absoluteFilePath();
                }
            }
        }

        if (false == p_3ds.isEmpty())
        {
            loadFile(p_3ds);
            loadTextureFile(p_image);
        }

        if (p_dirPath.isEmpty())
            return;

        QDir p_dir(p_dirPath);

        foreach(const QFileInfo &p_info, p_dir.entryInfoList())
        {
            if (p_info.isFile())
            {
                if (p_info.fileName().contains(QRegExp("3ds$",Qt::CaseInsensitive)))
                {
                    p_3ds = p_info.absoluteFilePath();
                } else if (p_info.fileName().contains(QRegExp("(png|jpg|jpeg)$",Qt::CaseInsensitive)))
                {
                    p_image = p_info.absoluteFilePath();
                }
            }
        }

        if (p_3ds.isEmpty())
            return;

        loadFile(p_3ds);
        loadTextureFile(p_image);

    }
}

void Viewer3ds::renderNode(Lib3dsNode *node)
{
    if(!m_file)
        return;

    for (Lib3dsNode* p=node->childs; p!=0; p=p->next)
        renderNode(p);

    if (node->type != LIB3DS_OBJECT_NODE)
        return;

    if (strcmp(node->name,"$$$DUMMY")==0)
        return;

    if (!node->user.d)
    {
        Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(m_file, node->name);
        if (!mesh)
            return;

        node->user.d = glGenLists(1);
        glNewList(node->user.d, GL_COMPILE);

        Lib3dsVector *normalL = new Lib3dsVector[3*mesh->faces];
        Lib3dsMaterial *oldMat = (Lib3dsMaterial *)uint(-1);

        Lib3dsMatrix M;
        lib3ds_matrix_copy(M, mesh->matrix);
        lib3ds_matrix_inv(M);
        glMultMatrixf(&M[0][0]);

        lib3ds_mesh_calculate_normals(mesh, normalL);

        for (unsigned int p=0; p<mesh->faces; ++p)
        {
            Lib3dsFace *f=&mesh->faceL[p];
            Lib3dsMaterial *mat=0;
            if (f->material[0])
                mat = lib3ds_file_material_by_name(m_file, f->material);

            if(oldMat != mat)
            {
                setMaterial(mat);
                oldMat = mat;
            }

            glBegin(GL_TRIANGLES);
            glNormal3fv(f->normal);
            for (int i=0; i<3; ++i)
            {
                glNormal3fv(normalL[3*p+i]);
                glTexCoord2f(mesh->texelL[f->points[i]][0], mesh->texelL[f->points[i]][1]);
                glVertex3fv(mesh->pointL[f->points[i]].pos);
            }
            glEnd();
        }

        delete[] normalL;

        glEndList();
    }

    if (node->user.d)
    {
        glPushMatrix();
        Lib3dsObjectData* d = &node->data.object;
        glMultMatrixf(&node->matrix[0][0]);
        glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
        glCallList(node->user.d);
        glPopMatrix();
    }
}

void Viewer3ds::setMaterial(Lib3dsMaterial *mat)
{
    if (!mat)
    {
        Lib3dsRgba a={0.2f, 0.2f, 0.2f, 1.0f};
        Lib3dsRgba d={0.8f, 0.8f, 0.8f, 1.0f};
        Lib3dsRgba s={0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT, a);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
        glMaterialfv(GL_FRONT, GL_SPECULAR, s);
        return;
    }

    if( mat->two_sided )
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);

    if (mat->texture1_map.name[0] && m_textureId == 0) {		// texture map?
        Lib3dsTextureMap *tex = &mat->texture1_map;
        emit sgnNeedTexture(this, QString(tex->name));
    }
    static GLfloat a[4]={0,0,0,1};
    float s;
    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
    s = pow(2, 10.0*mat->shininess);
    if (s>128.0)
        s=128.0;
    glMaterialf(GL_FRONT, GL_SHININESS, s);
}

void Viewer3ds::animate(int frame)
{
    if(!m_file)
        return;

    lib3ds_file_eval(m_file, frame % m_file->frames);
    initSceneLight();
}

bool Viewer3ds::addDefaultCamera()
{
    if(!m_file)
        return false;
    // Add some cameras that encompass the bounding box
    Lib3dsVector bmin, bmax;
    lib3ds_file_bounding_box_of_nodes(m_file, LIB3DS_TRUE, LIB3DS_FALSE, LIB3DS_FALSE, bmin, bmax);

    float sx = bmax[0] - bmin[0];
    float sy = bmax[1] - bmin[1];
    float sz = bmax[2] - bmin[2];
    float size = qMax(sx, sy); size = qMax(size, sz);
    float cx = (bmin[0] + bmax[0])/2;
    float cy = (bmin[1] + bmax[1])/2;
    float cz = (bmin[2] + bmax[2])/2;

    Lib3dsCamera *camera = lib3ds_camera_new("Camera_X");
    camera->target[0] = cx;
    camera->target[1] = cy;
    camera->target[2] = cz;
    memcpy(camera->position, camera->target, sizeof(camera->position));
    camera->position[0] = bmax[0] + 1.5 * qMax(sy,sz);
    camera->near_range = ( camera->position[0] - bmax[0] ) * .5;
    camera->far_range = ( camera->position[0] - bmin[0] ) * 2;
    lib3ds_file_insert_camera(m_file, camera);

    // Since lib3ds considers +Y to be into the screen, we'll put
    //this camera on the -Y axis, looking in the +Y direction.
    //
    camera = lib3ds_camera_new("Camera_Y");
    camera->target[0] = cx;
    camera->target[1] = cy;
    camera->target[2] = cz;
    memcpy(camera->position, camera->target, sizeof(camera->position));
    camera->position[1] = bmin[1] - 1.5 * qMax(sx,sz);
    camera->near_range = ( bmin[1] - camera->position[1] ) * .5;
    camera->far_range = ( bmax[1] - camera->position[1] ) * 2;
    lib3ds_file_insert_camera(m_file, camera);

    camera = lib3ds_camera_new("Camera_Z");
    camera->target[0] = cx;
    camera->target[1] = cy;
    camera->target[2] = cz;
    memcpy(camera->position, camera->target, sizeof(camera->position));
    camera->position[2] = bmax[2] + 1.5 * qMax(sx,sy);
    camera->near_range = ( camera->position[2] - bmax[2] ) * .5;
    camera->far_range = ( camera->position[2] - bmin[2] ) * 2;
    lib3ds_file_insert_camera(m_file, camera);

    camera = lib3ds_camera_new("Camera_ISO");
    camera->target[0] = cx;
    camera->target[1] = cy;
    camera->target[2] = cz;
    memcpy(camera->position, camera->target, sizeof(camera->position));
    camera->position[0] = bmax[0] + .75 * size;
    camera->position[1] = bmin[1] - .75 * size;
    camera->position[2] = bmax[2] + .75 * size;
    camera->near_range = ( camera->position[0] - bmax[0] ) * .5;
    camera->far_range = ( camera->position[0] - bmin[0] ) * 3;
    lib3ds_file_insert_camera(m_file, camera);

    return m_file->cameras != 0;
}

void Viewer3ds::addDefaultNodes()
{
    if(!m_file)
        return;

    for(Lib3dsMesh *mesh = m_file->meshes; mesh != NULL; mesh = mesh->next)
    {
        Lib3dsNode *node = lib3ds_node_new_object();
        qstrcpy(node->name, mesh->name);
        node->parent_id = LIB3DS_NO_PARENT;
        lib3ds_file_insert_node(m_file, node);
    }
}

void Viewer3ds::initCamera()
{
    if(!m_file|| !m_file->cameras)
        return;

    char* cameraName = m_file->cameras->name;

    Lib3dsNode* cameraNode = lib3ds_file_node_by_name(m_file, cameraName, LIB3DS_CAMERA_NODE);
    Lib3dsNode* targetNode = lib3ds_file_node_by_name(m_file, cameraName, LIB3DS_TARGET_NODE);
    Lib3dsCamera* pCamera  = lib3ds_file_camera_by_name(m_file, cameraName);
    if (!pCamera)
        return;

    Lib3dsVector targetCamera;
    if( targetNode != NULL )
        lib3ds_vector_copy(targetCamera, targetNode->data.target.pos);
    else
        lib3ds_vector_copy(targetCamera, pCamera->target);

    Lib3dsVector cameraPos;
    Lib3dsFloat cameraRoll(0.0f);
    Lib3dsFloat cameraFov(0.0f);
    if( cameraNode != NULL ) {
        cameraFov = cameraNode->data.camera.fov;
        cameraRoll = cameraNode->data.camera.roll;
        lib3ds_vector_copy(cameraPos, cameraNode->data.camera.pos);
    } else {
        cameraFov = pCamera->fov;
        cameraRoll = pCamera->roll;
        lib3ds_vector_copy(cameraPos, pCamera->position);
    }

    camera()->setFov( cameraFov );
    camera()->init( QVector3D(cameraPos[0], cameraPos[1], cameraPos[2]),
                    QVector3D(targetCamera[0], targetCamera[1], targetCamera[2]),
                    cameraRoll*180.0/3.14
                    );
}

void Viewer3ds::initSceneLight()
{
    if(!m_file)
        return;
    // Lights
    GLfloat amb[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat dif[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat spe[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat pos[] = {0.0, 0.0, 0.0, 1.0};
    int li=GL_LIGHT0;
    for (Lib3dsLight* l=m_file->lights; l; l=l->next)
    {
        glEnable(li);

        glLightfv(li, GL_AMBIENT,  amb);
        glLightfv(li, GL_DIFFUSE,  dif);
        glLightfv(li, GL_SPECULAR, spe);

        pos[0] = l->position[0];
        pos[1] = l->position[1];
        pos[2] = l->position[2];
        glLightfv(li, GL_POSITION, pos);

        if (!l->spot_light)
            continue;

        pos[0] = l->spot[0] - l->position[0];
        pos[1] = l->spot[1] - l->position[1];
        pos[2] = l->spot[2] - l->position[2];
        glLightfv(li, GL_SPOT_DIRECTION, pos);
        ++li;
    }
}

 // ns
