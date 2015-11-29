#ifndef VIEW_H
#define VIEW_H

#include <QGLWidget>
#include <QPoint>
#include <QVector3D>

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QContextMenuEvent;
class QTimerEvent;
class QEvent;
class QTouchEvent;

class Camera;

class View : public QGLWidget
{
    Q_OBJECT
    Q_PROPERTY(QVector3D rotate READ rotate WRITE setRotate)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(bool animate READ isAnimated WRITE setAnimated)
    Q_PROPERTY(bool freeMode READ isFreeMode WRITE setFreeMode)

public:
    enum VisualHint {
        VH_ShowAxis  = 1,
        VH_ShowPlane = 2
    };

    View(QWidget* parent = 0);
    ~View();

    Camera* camera() const;

    bool isAnimated() const;

    QVector3D rotate() const;

    qreal scale() const;

    qreal roll() const;

    bool isFreeMode() const;

    void setVisualHint( int hint);
    int visualHint() const;
    void toggleVisualHint(VisualHint hint);

public slots:
    void setAnimated(bool _isAnimate);

    void startAnimate();
    void stopAnimate();

    void setRotate(const QVector3D& _vec);

    void setScale(qreal _scale);

    void setRoll(qreal _roll);

    void setFreeMode(bool _isFreeMode);

    void shift(const QVector3D& _shift);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void timerEvent(QTimerEvent *event);
    bool event(QEvent* event);
    void gestureEvent(QTouchEvent *touchEvent);

    void initializeGL();
    void resizeGL(int _w, int _h);
    void paintGL();

    virtual void animate(int frame) { Q_UNUSED(frame); }
    virtual void drawScene() {}

    void drawAxis() const;
    void drawPlane() const;
    void drawArrow(float length, float radius, int nbSubdivisions = 10) const;

private:
    Camera* m_camera;
    int m_idTimerAnimate;
    int m_currentFrame;
    QPoint m_mousePos;
    qreal m_scaleTouched;
    qreal m_rollTouched;
    bool m_isTouchEvent;

    Q_DECLARE_FLAGS(VisualHints, VisualHint)
    VisualHints m_visualHints;
};

 // ns

#endif // VIEW_H
