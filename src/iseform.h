#ifndef ISEFORM_H
#define ISEFORM_H

#include <QWidget>
#include <QSortFilterProxyModel>

#include "isemodel.h"
#include "viewercontoller.h"

namespace Ui {
class IseForm;
}

class IseForm : public QWidget
{
    Q_OBJECT

public:
    explicit IseForm(ViewerContoller *_controller, QWidget *parent = 0);
    ~IseForm();

private slots:
    void filterChanged(const QString &_filter);
    void currentIndexChanged(const QModelIndex &_index);
    void refresh();

private:
    Ui::IseForm *ui;
    QPointer<ViewerContoller> m_contoller;
    QPointer<QSortFilterProxyModel> m_proxyModel;
    QPointer<ModelModel> m_model;
};

#endif // ISEFORM_H
