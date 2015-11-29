#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include <QObject>
#include "edititem.h"

class QWidget;

    class AbstractViewLoader : public QObject
    {
    public:
        AbstractViewLoader(QObject *parent) : QObject(parent) {}

        virtual void setMode(EditItem::E_MODE) = 0;
        virtual void load(const QByteArray& _model, const QByteArray &_texture) = 0;
        //virtual void load(const QString& _dynamicTable, const QString& _valuesPrimaryKey) = 0;
        virtual QWidget* createWidget() const = 0;
        virtual void loadAll(const QString &_model, const QString &_texture = QString()) { Q_UNUSED(_model) Q_UNUSED(_texture) }
    };


#endif // ABSTRACTVIEW_H
