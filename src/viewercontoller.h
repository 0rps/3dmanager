#ifndef VIEWERCONTOLLER_H
#define VIEWERCONTOLLER_H

#include <QPointer>

#include "view.h"
#include "viewer3dloader.h"
#include "globals.h"

class ViewerContoller: public QObject
{
    Q_OBJECT
public:
    ViewerContoller(QGLWidget *_view, Viewer3dLoader *_loader, QObject *parent);

    void loadModel(const Model &_model);
    void loadModel(const long &_id);
    void loadModel(const QString &_name, const QString &_texture);
    void loadModel(const QByteArray &_model, const QByteArray &_texture);

    QImage image() const;



private:
    QPointer<QGLWidget> m_widget;
    QPointer<Viewer3dLoader> m_loader;
};

#endif // VIEWERCONTOLLER_H
