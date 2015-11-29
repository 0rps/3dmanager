#include "modelsform.h"
#include "ui_modelsform.h"

#include <QLinkedList>

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

#include "clipper.h"
#include "viewercontoller.h"

#include "db.h"

#define PICTURE_SIZE QSize(800,500)
#define PICTURE_FOLDER "D:\\model_test\\new\\added"

ModelsForm::ModelsForm(ViewerContoller *_controller ,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelsForm),
    m_diffModel(new ModelModel),
    m_controller(_controller),
    m_clipper(PICTURE_SIZE, 10, 10)
{
    ui->setupUi(this);

    connect(ui->m_refreshButton, SIGNAL(clicked()), this, SLOT(handleDiff()) );
    connect(ui->m_saveDiffModels, SIGNAL(clicked()), this, SLOT(saveModels()) );
    connect(ui->m_savePictures, SIGNAL(clicked()), this, SLOT(savePictures()) );
    connect(ui->m_saveOnePicture, SIGNAL(clicked()), this, SLOT(saveOnePicture()) );
    connect(ui->m_saveOneModel, SIGNAL(clicked()), this, SLOT(saveOneModel()) );
    connect(ui->m_removeSelectedDiffRow, SIGNAL(clicked()), this, SLOT(removeFromDiff()) );
    //connect(ui->m_savePicture, SIGNAL(clicked()), this, SLOT(saveOnePicture()) );
    connect(ui->m_saveDiffModels, SIGNAL(clicked()), this, SLOT(saveModels()) );
    //connect(ui->m_removeModel, SIGNAL(clicked()), this, SLOT(removeFromDiff()) );

    m_diffProxy = new QSortFilterProxyModel();
    m_diffProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_diffProxy->setSourceModel(m_diffModel.data());

    ui->diffDbView->setModel(m_diffProxy);

    connect(ui->diffDbView, SIGNAL(clicked(QModelIndex)), this, SLOT(currentModelChanged(QModelIndex)) );
    connect(ui->m_diffFilter, SIGNAL(textChanged(QString)), this, SLOT(setDiffFilter(QString)) );

    update();

}

ModelsForm::~ModelsForm()
{
    delete ui;
}

void ModelsForm::handleDiff()
{
    QList<Model> p_models = Db::selectIseModels();
    QList<long> p_localIds = Db::selectIdFromStatDb();
    QHash<long, bool> p_hash;
    foreach(const int _id, p_localIds)
    {
        p_hash.insert(_id, true);
    }

    QList<Model> p_diff;

    foreach(const Model &_model, p_models)
    {
        if (false == p_hash.contains(_model.united_id))
            p_diff.append(_model);
    }

    m_diffModel->setModels(p_diff);
}


void ModelsForm::removeFromDiff()
{
    QModelIndexList p_list = ui->diffDbView->selectionModel()->selectedIndexes();
    if (p_list.size() > 0)
    {
        for (int i = p_list.size()-1; i >= 0; i--)
        {
            ui->diffDbView->model()->removeRow(p_list[i].row());
        }
    }
}

void ModelsForm::saveOnePicture()
{
    QModelIndexList p_list = ui->diffDbView->selectionModel()->selectedIndexes();

    if (p_list.size() > 0)
    {
        savePicture(p_list[0].data(IR_DATA).value<Model>(), PICTURE_FOLDER);
    }
}

void ModelsForm::savePictures()
{
    QAbstractItemModel *p_model = ui->diffDbView->model();
    int rowCount = p_model->rowCount();

    for (int i = 0; i < rowCount; i++)
    {
        QModelIndex p_index = p_model->index(i, 0);
        if (p_index.isValid()) {
            Model p_model = p_index.data(IR_DATA).value<Model>();
            savePicture(p_model, PICTURE_FOLDER);

        }
    }

    /// commented part is save pictures from folder to base
    /// DO NOT DELETE!!!!

//    QDir p_dir("D:\\model_test");
//    QFileInfoList p_list  = p_dir.entryInfoList();

//    QList<Model> p_models = m_sourceModel->getModels();
//    QHash<long, Model> p_hash;
//    foreach(const Model &_model, p_models)
//    {
//        p_hash.insert(_model.united_id, _model);
//    }

//    QList<Model> p_res;

//    foreach(const QFileInfo &_info, p_list)
//    {
//        if (_info.isFile())
//        {
//            QStringList p_strs = _info.fileName().split("_");
//            if (p_strs.length() > 0)
//            {
//                long num = p_strs[0].toLong();
//                QImage p_image(_info.absoluteFilePath());
//                if (! p_image.isNull())
//                {
//                    if (p_hash[num].image.isNull() ) {
//                        p_hash[num].image = p_image;
//                        p_res.append(p_hash[num]);
//                    }
//                }
//            }
//        }
//    }

//    m_diffModel->setModels(p_res);



}

void ModelsForm::saveModels()
{
    QList<Model> p_models = m_diffModel->getModels();

    Db::addModelsToStatDb(p_models);

}

void ModelsForm::saveOneModel()
{
    QModelIndexList p_list = ui->diffDbView->selectionModel()->selectedIndexes();

    if (p_list.size() > 0)
    {
        QModelIndex p_index = p_list[0];
        Model p_model = p_index.data(IR_DATA).value<Model>();
        p_model.image = getImage(p_model);
        QList<Model> p_models;
        p_models << p_model;
        if ( Db::addModelsToStatDb(p_models) > 0) {
            ui->diffDbView->model()->removeRow(p_index.row());

            info("Model " + p_model.name + " saved to statdbase");
        } else
            error("Coudnot save, something wrong");
    }
}


void ModelsForm::setDiffFilter(const QString &_str)
{
    m_diffProxy->setFilterFixedString(_str);
}

void ModelsForm::currentModelChanged(const QModelIndex &_index)
{
    if (_index.isValid())
    {
        Model p_model = _index.data(IR_DATA).value<Model>();
        loadModelTo3d(p_model);
    }
}

bool ModelsForm::savePicture(const Model &_model, const QString _path)
{
    QString p_path = QString("%1\\%2_%3.png").arg(_path).arg(_model.united_id).arg(_model.name);
    if (QFile::exists(p_path))
    {
        error("Cannot save picture, path is no empty " + p_path);
        return false;
    }

    info("Saving " + _model.name);
    QImage p_image = getImage(_model);
    p_image.save(p_path, "PNG", 98);

    return true;
}

QImage ModelsForm::getImage(const Model &_model)
{
    loadModelTo3d(_model);

    QImage p_image = m_controller->image();
    return m_clipper.clip(p_image);
}

void ModelsForm::loadModelTo3d(const Model &_model)
{
    m_controller->loadModel(_model);
}

void ModelsForm::info(const QString &_str)
{
    //ui->m_console->append(QString("INFO: ") + _str);
}

void ModelsForm::error(const QString &_str)
{
    //ui->m_console->append(QString("ERROR: ") + _str);
}

