#include <qgl.h>

#ifdef _WIN32
#   include <Windows.h>
#   include <GL/GLU.h>
#else
#   include <GL/glu.h>
#endif

#include <QQuaternion>
#include <QSize>

#include "camera.h"

Camera::Camera(const QSize& sizeViewport) :
    m_cameraFov(45.0f),
    m_nearClip(0.001f),
    m_farClip(1e16f),
    m_viewPortSize(sizeViewport),
    m_scale(1.0),
    m_roll(0.0),
    m_right( QVector3D(1, 0, 0) ),
    m_up( QVector3D(0, 0, 1) ),
    m_isFreeMode(false),
    m_sceneRadius(1.0)
{
    resizeViewport( sizeViewport.width(), sizeViewport.height() );
}

float Camera::fov() const
{
    return m_cameraFov;
}

void Camera::setFov(float fov)
{
    m_cameraFov = fov;
}

qreal Camera::scale() const
{
    return m_scale;
}

void Camera::setScale(qreal _scale)
{
    QVector3D viewDirection = QVector3D::crossProduct(m_right, m_up);
    viewDirection *= m_scale/_scale * (m_cameraPos - m_targetCamera).length() * 0.1;

    if(_scale < m_scale)
        m_cameraPos += viewDirection;
    else
        m_cameraPos -= viewDirection;

    matrixByAxisCamera(m_cameraMatrix, m_cameraPos, m_right, m_up);

    m_scale = _scale;
}

QVector3D Camera::rotate() const
{
    return m_rotate;
}

void Camera::setRotate(const QVector3D &_vec)
{
    QVector3D diffRotate = m_rotate - _vec;

    if(m_isFreeMode)
    {
        QQuaternion quaRight = QQuaternion::fromAxisAndAngle(m_right, -diffRotate.x() );
        QQuaternion quaUp    = QQuaternion::fromAxisAndAngle(m_up, -diffRotate.z() );

        m_right = quaUp.rotatedVector(m_right);
        m_up    = quaRight.rotatedVector(m_up);
    } else {
        QQuaternion quaRight = QQuaternion::fromAxisAndAngle(m_right, diffRotate.x());
        QQuaternion quaUp    = QQuaternion::fromAxisAndAngle(m_up, diffRotate.z());
        QQuaternion qua = quaUp*quaRight;

        QVector3D rightRadiusVector = relativeRotate(m_right + m_cameraPos, qua, m_targetCamera);
        QVector3D upRadiusVector    = relativeRotate(m_up    + m_cameraPos, qua, m_targetCamera);
        m_cameraPos = relativeRotate(m_cameraPos, qua, m_targetCamera);

        m_right = (rightRadiusVector - m_cameraPos).normalized();
        m_up    = (upRadiusVector - m_cameraPos).normalized();
    }

    matrixByAxisCamera(m_cameraMatrix, m_cameraPos, m_right, m_up);

    m_rotate = _vec;
}

qreal Camera::roll() const
{
    return m_roll;
}

void Camera::setRoll(qreal _roll)
{
    qreal diffRoll = m_roll - _roll;

    QVector3D dir = (m_targetCamera - m_cameraPos);
    QQuaternion qua = QQuaternion::fromAxisAndAngle(dir, -diffRoll);
    m_right = qua.rotatedVector( m_right );
    m_up = qua.rotatedVector( m_up );

    matrixByAxisCamera(m_cameraMatrix, m_cameraPos, m_right, m_up);

    m_roll = _roll;
}

void Camera::shift(const QVector3D &shift)
{
    qreal k = (m_cameraPos - m_targetCamera).length() * 0.01;
    m_cameraPos += -k*shift.x()* m_right +
                    k*shift.y()* m_up;

    if(!m_isFreeMode)
        calcCameraAxis();

    matrixByAxisCamera(m_cameraMatrix, m_cameraPos, m_right, m_up);
}

qreal Camera::isFreeMode() const
{
    return m_isFreeMode;
}

void Camera::setFreeMode(bool _isFreeMode)
{
    m_isFreeMode = _isFreeMode;
    if(!m_isFreeMode)
    {
        calcCameraAxis();
        matrixByAxisCamera(m_cameraMatrix, m_cameraPos, m_right, m_up);
    }
}

void Camera::setAnimatedRotation(const QVector3D& rot)
{
    m_animatedRotation = rot;
}

qreal Camera::sceneRadius() const
{
    return m_sceneRadius;
}

QVector3D Camera::relativeRotate(const QVector3D& pos, const QQuaternion& qua, const QVector3D& target) const
{
    return qua.rotatedVector( pos - target ) + target;
}

void Camera::init(const QVector3D &pos, const QVector3D &target, qreal roll)
{
    m_cameraPos = pos;
    m_targetCamera = target;
    m_sceneRadius = (pos - target).length();

    calcCameraAxis();

    m_roll = 0.0; /// NOTE: clean, because setRoll use difference
    setRoll( roll );
}

void Camera::calcCameraAxis()
{
    QVector3D dir = (m_targetCamera - m_cameraPos).normalized();

    QVector3D up;
    if(dir.x() != 0. || dir.y() != 0.)
        up.setZ( 1.0 );
    else
        up.setX( -1.0 );// Special case:  looking straight up or down z axis

    m_right = QVector3D::crossProduct(dir, up).normalized();
    m_up = QVector3D::crossProduct(m_right, dir).normalized();
}

void Camera::matrixByAxisCamera(QMatrix4x4& result, const QVector3D& pos,
                                                    const QVector3D& vRightNormalized,
                                                    const QVector3D& vUpNormalized) const
{
    QVector3D right = vRightNormalized;
    QVector3D up = vUpNormalized;
    QVector3D dir = QVector3D::crossProduct(up, right);

    result.setToIdentity();

    result.setRow(0, right);
    result.setRow(1, dir);
    result.setRow(2, up);

    result.translate( -pos );
}

void Camera::loadModelViewMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-90, 1.0,0,0); /// NOTE: standart 3ds TODO: remove this

#if defined(QT_NO_FPU) || defined(QT_ARCH_ARM) || defined(QT_ARCH_WINDOWSCE) || defined(QT_ARCH_SYMBIAN)
    glMultMatrixf(m_cameraMatrixMV.data());
#else
    glMultMatrixd(m_cameraMatrix.data());
#endif
}

void Camera::loadProjectionMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective( m_cameraFov, (float)m_viewPortSize.width()/m_viewPortSize.height(), m_nearClip, m_farClip);
}

void Camera::resizeViewport(int _w, int _h)
{
    m_viewPortSize = QSize( _w, _h);
    loadProjectionMatrix();
}

void Camera::animate()
{
    if(m_animatedRotation.isNull() || m_isFreeMode)
        return;

    setRotate( rotate() - m_animatedRotation );
}

 // ns

