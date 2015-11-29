#ifndef MODELFORM_H
#define MODELFORM_H

#include <QWidget>

#include "model.h"

#include "viewercontoller.h"

namespace Ui {
class ModelForm;
}

class ModelForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit ModelForm(const Model &_model, ViewerContoller *_controller, const bool _isEdit, QWidget *parent = 0);

    ~ModelForm();

    Model model() const;

private slots:
    void addToAlbumChecked( const bool _isChecked);
    void isBugChecked(const bool _isChecked);
    void nameChanged(const QString &_name);

    void maxButtonPushed();
    void tridsButtonPushed();
    void textureButtonPushed();

    void refreshModelAtViewer();
    void refreshPicture();

    void armyChanged(const int _index);

private:
    void setUiEdit(const bool _isEdit);
    void uiValuesUpdate();

private:
    Ui::ModelForm *ui;
    Model *m_model;

    QPointer<ViewerContoller> m_controller;
};

#endif // MODELFORM_H
