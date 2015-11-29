#ifndef MODELSFORM_H
#define MODELSFORM_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QGLWidget>
#include <QPointer>

#include "viewercontoller.h"
#include "clipper.h"
#include "isemodel.h"

namespace Ui {
class ModelsForm;
}

class ModelsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ModelsForm(ViewerContoller *_controller, QWidget *parent = 0);
    ~ModelsForm();

private slots:
    void handleDiff();
    void removeFromDiff();
    void saveOnePicture();
    void savePictures();
    void saveModels();
    void saveOneModel();

    void setDiffFilter(const QString &_str);

    void currentModelChanged(const QModelIndex &_index);

    bool savePicture(const Model &_model, const QString _path);
    void loadModelTo3d(const Model &_model);

protected:
    void info(const QString &_str);
    void error(const QString &_srt);

private:
    QImage getImage(const Model &_model);

private:
    Ui::ModelsForm *ui;

    QSortFilterProxyModel *m_diffProxy;
    QScopedPointer<ModelModel> m_diffModel;
    QPointer<ViewerContoller> m_controller;

    Clipper m_clipper;
};

#endif // MODELSFORM_H
