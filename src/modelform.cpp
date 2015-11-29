#include "modelform.h"
#include "ui_modelform.h"

#include <QFileDialog>
#include <QFile>

#include "clipper.h"
#include "db.h"
#include "log.h"
#include "armyitemmodel.h"

#define PICTURE_SIZE QSize(800,500)

ModelForm::~ModelForm()
{
    delete ui;
}

Model ModelForm::model() const
{
    return m_model;
}

void ModelForm::addToAlbumChecked(const bool _isChecked)
{
    m_model->setShow(_isChecked);
}

void ModelForm::setUiEdit(const bool _isEdit)
{
    ui->m_armyBox->setEnabled(_isEdit);
    ui->m_nameEdit->setReadOnly(_isEdit);
    ui->m_addToAlbumCheck->setEnabled(_isEdit);
    ui->m_isBugCheck->setEnabled(_isEdit);
    ui->m_load3dsButton->setEnabled(_isEdit);
    ui->m_loadMaxButton->setEnabled(_isEdit);
    ui->m_loadTextureButton->setEnabled(_isEdit);
    ui->m_refreshImageButton->setEnabled(_isEdit);
}

void ModelForm::uiValuesUpdate()
{
    ui->m_nameEdit->setText( m_model->name() );
    ui->m_addToAlbumCheck->setChecked( m_model->isShow() );
    ui->m_imageLabel->setPixmap( QPixmap::fromImage(m_model->image()) );
    ui->m_inClassifierCheck->setCheckable(m_model->inClassifier());
    ui->m_isBugCheck->setChecked(m_model->isBug());

    QString p_has3ds = m_model->has3ds() ? "Присутствует":"Отсутствует";
    QString p_hasmax = m_model->hasMax() ? "Присутствует":"Отсутствует";
    QString p_hasTexture = m_model->hasTexture() ? "Присутствует":"Отсутствует";

    ui->m_imageLabel->setPixmap( QPixmap::fromImage(m_model->image()) );
    ui->m_maxLabel->setText(p_hasmax);
    ui->m_3dsLabel->setText(p_has3ds);
    ui->m_textureLabel->setText(p_hasTexture);
}

void ModelForm::refreshPicture()
{
    Clipper p_clipper(PICTURE_SIZE, 10, 10);
    m_controller->loadModel(m_model->trids(), m_model->texture());

    m_model->setImage(p_clipper.clip(m_controller->image()));
    ui->m_imageLabel->setPixmap(QPixmap::fromImage(m_model->image()));
}

void ModelForm::armyChanged(const int _index)
{
    m_model->setArmy( ui->m_armyBox->itemData(_index, ArmyItemModel::MR_TYPE).toInt() );
}

ModelForm::ModelForm(const Model &_model, ViewerContoller *_controller, const bool _isEdit, QWidget *parent = 0):
    QWidget(parent),
    ui(new Ui::ModelForm),
    m_model(_model),
    m_controller(_controller)
{
    ui->setupUi(this);

    setUiEdit(_isEdit);

    connect(ui->m_addToAlbumCheck, SIGNAL(clicked(bool)), this,  SLOT(addToAlbumChecked(bool)));
    connect(ui->m_isBugCheck, SIGNAL(clicked(bool)), this, SLOT(isBugChecked(bool)));

    connect(ui->m_load3dsButton, SIGNAL(clicked()), this, SLOT(tridsButtonPushed()) );
    connect(ui->m_loadMaxButton, SIGNAL(clicked()), this, SLOT(maxButtonPushed()) );
    connect(ui->m_loadTextureButton, SIGNAL(clicked()), this, SLOT(textureButtonPushed()) );

    connect(ui->m_refreshImageButton, SIGNAL(clicked()), this, SLOT(refreshPicture()) );
    connect(ui->m_refresh3DButton, SIGNAL(clicked()), this, SLOT(refreshModelAtViewer()) );

    connect(ui->m_armyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(armyChanged(int)) );

    if (_isEdit == false)
    {
        uiValuesUpdate();
        return;
    }

    if (false == _model.isValid())
    {
        if (m_model->inClassifier())
        {
            m_model->set3ds(Db::readModelFromIse(_unitedId));
            m_model->setTexture(Db::readTextureFromIse(_unitedId));
            refreshPicture();
        }
    }

    uiValuesUpdate();
}


void ModelForm::maxButtonPushed()
{
    QString p_filename = QFileDialog::getOpenFileName(this, "Открыть max файл",
                                 "", "Файлы max (*.max)");
    if (false == p_filename.isEmpty())
    {
        QFile p_file(p_filename);
        if (p_file.open(QFile::ReadOnly))
        {
            m_model->setMax(p_file.readAll());
            uiValuesUpdate();
        } else
        {
            logError() << "Couldnot open file " << p_filename;
        }
    }
}

void ModelForm::tridsButtonPushed()
{
    QString p_filename = QFileDialog::getOpenFileName(this, "Открыть max файл",
                                 "", "Файлы 3ds (*.3ds)");
    if (false == p_filename.isEmpty())
    {
        QFile p_file(p_filename);
        if (p_file.open(QFile::ReadOnly))
        {
            m_model->setMax(p_file.readAll());
            uiValuesUpdate();
        } else
        {
            logError() << "Couldnot open file " << p_filename;
        }
    }
}

void ModelForm::textureButtonPushed()
{
    QString p_filename = QFileDialog::getOpenFileName(this, "Открыть texture файл",
                                 "", "Файлы jpeg, png (*.*)");
    if ( false == p_filename.isEmpty() )
    {
        QFile p_file(p_filename);
        if (p_file.open(QFile::ReadOnly))
        {
            m_model->setMax(p_file.readAll());
            uiValuesUpdate();
        } else
        {
            logError() << "Couldnot open file " << p_filename;
        }
    }
}

void ModelForm::refreshModelAtViewer()
{
    m_controller->loadModel(m_model->trids(), m_model->texture());
}


void ModelForm::nameChanged(const QString &_name)
{
    m_model->setName(_name);
}


void ModelForm::isBugChecked(const bool _isChecked)
{
    m_model->setBug(_isChecked);
}
