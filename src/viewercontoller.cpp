#include "viewercontoller.h"

ViewerContoller::ViewerContoller(QGLWidget *_view, Viewer3dLoader *_loader, QObject *parent):
    QObject(parent),
    m_loader(_loader),
    m_widget(_view)
{

}

void ViewerContoller::loadModel(const Model &_model)
{
    QString p_str = QObject::tr("5=%1|,|11=1|,|12=2009|,|13=Российская Федерация|,|").arg(_model.united_id);
    m_loader->load("",  p_str);
}

void ViewerContoller::loadModel(const long &_id)
{
    QString p_str = QObject::tr("5=%1|,|11=1|,|12=2009|,|13=Российская Федерация|,|").arg(_id);
    m_loader->load("",  p_str);
}

void ViewerContoller::loadModel(const QString &_name, const QString &_texture)
{
    m_loader->loadAll(_name, _texture);
}

void ViewerContoller::loadModel(const QByteArray &_model, const QByteArray &_texture)
{
    m_loader->load(_model, _texture);
}


QImage ViewerContoller::image() const
{
    return m_widget->grabFrameBuffer();
}
