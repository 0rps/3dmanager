#include <math.h>

#include <qgl.h>

#ifdef _WIN32
#   include <Windows.h>
#   include <GL/GLU.h>
#else
#   include <GL/glu.h>
#endif

#include <QMouseEvent>
#include <QDebug>
#include <QLineF>

#include "camera.h"
#include "view.h"

#include "clipper.h"

View::View(QWidget* parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::AlphaChannel), parent),
    m_camera( new Camera(size()) ),
    m_idTimerAnimate(0),
    m_currentFrame(0),
    m_scaleTouched(0.0),
    m_rollTouched(0.0),
    m_isTouchEvent(false),
    m_visualHints(0)
{
    setMinimumSize(150, 150);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_AcceptTouchEvents);
}

View::~View()
{
    stopAnimate();
    delete m_camera;
}

Camera *View::camera() const
{
    return m_camera;
}

bool View::isAnimated() const
{
    return m_idTimerAnimate != 0;
}

QVector3D View::rotate() const
{
    return m_camera->rotate();
}

void View::setRotate(const QVector3D &_vec)
{
    makeCurrent();
    m_camera->setRotate( _vec );
    updateGL();
}

qreal View::scale() const
{
    return m_camera->scale();
}

qreal View::roll() const
{
    return m_camera->roll();
}

bool View::isFreeMode() const
{
    return m_camera->isFreeMode();
}

void View::setVisualHint(int hint)
{
    m_visualHints = (VisualHints)hint;
}

int View::visualHint() const
{
    return m_visualHints;
}

void View::toggleVisualHint(View::VisualHint hint)
{
    if(m_visualHints & hint)
        m_visualHints &= ~hint;
    else
        m_visualHints |= hint;

    updateGL();
}

void View::setScale(qreal _scale)
{
    makeCurrent();
    m_camera->setScale(_scale);
    updateGL();
}

void View::setRoll(qreal _roll)
{
    makeCurrent();
    m_camera->setRoll(_roll);
    updateGL();
}

void View::setFreeMode(bool _isFreeMode)
{
    makeCurrent();
    m_camera->setFreeMode(_isFreeMode);
    updateGL();
}

void View::shift(const QVector3D &_shift)
{
    makeCurrent();
    m_camera->shift(_shift);
    updateGL();
}


void View::mousePressEvent(QMouseEvent* event)
{
    m_mousePos = event->pos();
    camera()->setAnimatedRotation( QVector3D() );
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isTouchEvent)
        return;

    const qreal c_mouseFactor = 0.2;
    QPoint newPos = event->pos();

    if((event->buttons() & Qt::LeftButton) && (event->modifiers() & Qt::ControlModifier))
    {
        QPoint center(width()/2, height()/2);
        QLineF oldLine( center, m_mousePos);
        QLineF newLine( center, newPos);

        qreal angle = oldLine.angleTo(newLine);
        if(angle > 180.0)
            angle = angle - 360.0;
        setRoll( roll() +  angle*c_mouseFactor*2  );
    }
    else if(event->buttons() & Qt::LeftButton)
    {
        QVector3D diffRotate = QVector3D(newPos.y() - m_mousePos.y(),
                                         0,
                                         newPos.x() - m_mousePos.x()) * c_mouseFactor;

        setRotate( rotate() + diffRotate );

        const qreal c_treshold = 15.0;
        const qreal c_speedFactor = 0.5;
        if(diffRotate.lengthSquared() > c_treshold)
            camera()->setAnimatedRotation( -diffRotate*c_speedFactor );
        else
            camera()->setAnimatedRotation( QVector3D() );
    }
    else if((event->buttons() & Qt::MidButton) && isFreeMode())
    {
        QVector3D diffShift = c_mouseFactor* QVector3D(newPos.x() - m_mousePos.x(),
                                         newPos.y() - m_mousePos.y(),
                                         0 );//newPos.x() - m_mousePos.x()) * c_mouseFactor;
        shift(diffShift);
    }

    m_mousePos = newPos;
}

void View::wheelEvent(QWheelEvent *event)
{
    const qreal c_wheelFactor = 0.8;
    if(event->delta() < 0)
        setScale( scale()*c_wheelFactor );
    else
        setScale( scale()/c_wheelFactor );
}

void View::keyPressEvent(QKeyEvent *e)
{
    const qreal c_keyFactor = 2.0;
    switch(e->key())
    {
       case Qt::Key_Space:  setAnimated( !isAnimated() ); break;
       case Qt::Key_Up   :
                if(e->modifiers() & Qt::ControlModifier )
                    setRoll( roll() + c_keyFactor );
                else
                    setRotate( rotate() + QVector3D(-c_keyFactor , 0, 0) );
                break;
       case Qt::Key_Down :
                if(e->modifiers() & Qt::ControlModifier )
                    setRoll( roll() - c_keyFactor );
                else
                    setRotate( rotate() + QVector3D(c_keyFactor , 0, 0) );
                break;

       case Qt::Key_Left :  setRotate( rotate() + QVector3D(0           , 0, -c_keyFactor) ); break;
       case Qt::Key_Right:  setRotate( rotate() + QVector3D(0           , 0, c_keyFactor) ); break;
       case Qt::Key_Plus :  setScale( scale() * 1.2 ); break;
       case Qt::Key_Minus:  setScale( scale() / 1.2 ); break;
       case Qt::Key_A    :  if(e->modifiers() & Qt::ControlModifier )
                                 toggleVisualHint( VH_ShowAxis);
                            break;
       case Qt::Key_P    :  if(e->modifiers() & Qt::ControlModifier )
                                toggleVisualHint( VH_ShowPlane);
                            break;
       default:
            break;
    }
}

bool View::event(QEvent *event)
{
    if(QEvent::TouchBegin <= event->type() && event->type() <= QEvent::TouchEnd)
    {
        gestureEvent(static_cast<QTouchEvent *>(event));
        return true;
    }

    return QGLWidget::event( event );
}

void View::gestureEvent(QTouchEvent *touchEvent)
{
    if(touchEvent->type() == QEvent::TouchBegin)
    {
        m_scaleTouched = m_camera->scale();
        m_rollTouched  = m_camera->roll();
    }

    QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
    if (touchPoints.count() != 2)
    {
        m_isTouchEvent = false;
        return;
    }

    // determine scale factor
    const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
    const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
    QLineF current(touchPoint0.pos(), touchPoint1.pos());
    QLineF start(touchPoint0.startPos(), touchPoint1.startPos());

    qreal currentScaleFactor = current.length() / start.length();
    const qreal c_coeffScaled = 0.3;
    currentScaleFactor = 1.0 +  (currentScaleFactor - 1.0)*c_coeffScaled;
    setScale(  m_scaleTouched * currentScaleFactor );

    // determine roll camera
    qreal currentRoll = start.angleTo( current );
    setRoll( m_rollTouched + currentRoll );

    m_isTouchEvent = touchEvent->type() != QEvent::TouchEnd;
}

void View::setAnimated(bool _isAnimate)
{
    if(_isAnimate)
        startAnimate();
    else
        stopAnimate();
}

void View::startAnimate()
{
    updateGL();
    if(m_idTimerAnimate)
        return;

    m_currentFrame = 0;
    m_idTimerAnimate = startTimer(20);
}

void View::stopAnimate()
{
    if(m_idTimerAnimate == 0)
        return;

    killTimer(m_idTimerAnimate);
    m_idTimerAnimate = 0;
}


void View::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != m_idTimerAnimate)
        return;

    makeCurrent();

    ++m_currentFrame;
    animate(m_currentFrame);

    camera()->animate();

    updateGL();
}


void View::initializeGL()
{
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_POLYGON_SMOOTH);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}

void View::resizeGL(int _w, int _h)
{
    glViewport(0, 0, (GLint)_w, (GLint)_h);

    m_camera->resizeViewport(_w, _h);
}


void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_camera->loadModelViewMatrix();
    drawScene();

    if( m_visualHints & VH_ShowAxis )
        drawAxis();

    if( m_visualHints & VH_ShowPlane )
        drawPlane();
}

void View::drawAxis() const
{
    float length = camera()->sceneRadius();
    float colorZ[4] = {0.8f, 0.8f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorZ);
    drawArrow(length, 0.01*length);

    float colorX[4] = {1.0f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorX);
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);
    drawArrow(length, 0.01*length);
    glPopMatrix();

    float colorY[4] = {0.8f, 1.0f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorY);
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    drawArrow(length, 0.01*length);
    glPopMatrix();
}

void View::drawArrow(float length, float radius, int nbSubdivisions) const
{
    static GLUquadric* quadric = gluNewQuadric();

    if (radius < 0.0)
        radius = 0.05 * length;

    const float head = 2.5*(radius / length) + 0.1;
    const float coneRadiusCoef = 4.0 - 5.0 * head;

    gluCylinder(quadric, radius, radius, length * (1.0 - head/coneRadiusCoef), nbSubdivisions, 1);
    glTranslatef(0.0, 0.0, length * (1.0 - head));
    gluCylinder(quadric, coneRadiusCoef * radius, 0.0, head * length, nbSubdivisions, 1);
    glTranslatef(0.0, 0.0, -length * (1.0 - head));
}

void View::drawPlane() const
{
    float size = camera()->sceneRadius();
    const float step = pow(10.0, floor(log10(size)) -1.0);
    const int nbSubdivisions = ceil(size/step);
    size = nbSubdivisions*step;

    GLboolean isLighting;
    glGetBooleanv(GL_LIGHTING, &isLighting);
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
    for (int i = 0;i <= nbSubdivisions; ++i)
    {
        const float pos = size*(2.0*i/nbSubdivisions-1.0);
        glVertex2f(pos, -size);
        glVertex2f(pos, +size);
        glVertex2f(-size, pos);
        glVertex2f( size, pos);
    }
    glEnd();

    if(isLighting)
        glEnable(GL_LIGHTING);
}

 // ns
