#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>

class QQuaternion;
class QSize;

class Camera
{
    friend class View;
public:
    Camera(const QSize& sizeViewport);

    void init(const QVector3D& pos, const QVector3D& target, qreal roll);

    float fov() const;
    void setFov(float fov);

    qreal scale() const;
    void setScale(qreal _scale);

    QVector3D rotate() const;
    void setRotate(const QVector3D& _vec);

    qreal roll() const;
    void setRoll(qreal _roll);

    void shift(const QVector3D& shift);

    qreal isFreeMode() const;
    void setFreeMode(bool _isFreeMode);

    void setAnimatedRotation(const QVector3D& rot);

    qreal sceneRadius() const;

    void animate();

protected:
    void loadModelViewMatrix();
    void loadProjectionMatrix();
    void resizeViewport(int _w, int _h);

private:
    void matrixByAxisCamera(QMatrix4x4& result, const QVector3D& pos,
                            const QVector3D& vRightNormalized,
                            const QVector3D& vUpNormalized) const;

    QVector3D relativeRotate(const QVector3D& pos,
                             const QQuaternion& qua,
                             const QVector3D& target) const;
    void calcCameraAxis();

private:
    float m_cameraFov;
    float m_nearClip;
    float m_farClip;
    QSize m_viewPortSize;

    qreal m_scale;
    QVector3D m_rotate;
    qreal m_roll;

    QVector3D m_right;
    QVector3D m_up;
    QVector3D m_cameraPos;
    QVector3D m_targetCamera;
    QMatrix4x4 m_cameraMatrix;
    bool m_isFreeMode;

    qreal m_sceneRadius;
    QVector3D m_animatedRotation;
};

 //ns

#endif // CAMERA_H
