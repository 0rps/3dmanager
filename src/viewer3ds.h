#ifndef VIEWER3D_H
#define VIEWER3D_H

#include "view.h"

struct Lib3dsFile;
struct Lib3dsNode;
struct Lib3dsLight;
struct Lib3dsMaterial;

class Viewer3ds : public View
{
    Q_OBJECT
public:
    Viewer3ds(QWidget* parent = 0);
    ~Viewer3ds();

    bool loadFile(QIODevice &_ioDevice);
    bool loadTextureFile(QIODevice &_ioDevice);

public slots:
    bool loadFile(const QString& _fileName);
    bool loadTextureFile(const QString& _fileName);

    bool loadFile(const QByteArray &_data);
    bool loadTextureFile(const QByteArray &_data);

signals:
    void sgnNeedContextMenu(Viewer3ds* _sender, const QPoint& _globalPos);
    void sgnNeedTexture(Viewer3ds* _sender, const QString& _pathTexture);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void animate(int frame);
    void drawScene();
    void dragEnterEvent(QDragEnterEvent *_event);
    void dropEvent(QDropEvent *_event);

private:
    void initCamera();
    void initSceneLight();
    void renderNode(Lib3dsNode *node);
    bool addDefaultCamera();
    void addDefaultNodes();
    void setMaterial(Lib3dsMaterial *mat);
    void clean();
    void cleanTexture();

private:
    Lib3dsFile *m_file;  
    GLuint m_textureId;
    int m_currentFrame;
};

 // ns

#endif // VIEWER3D_H
