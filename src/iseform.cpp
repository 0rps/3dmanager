#include "iseform.h"
#include "ui_iseform.h"

#include "db.h"

IseForm::IseForm(ViewerContoller *_controller, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IseForm),
    m_contoller(_controller)
{
    ui->setupUi(this);
    m_proxyModel = new QSortFilterProxyModel(ui->m_iseView);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_model = new ModelModel(m_proxyModel);
    m_proxyModel->setSourceModel(m_model);

    ui->m_iseView->setModel(m_proxyModel);

    connect(ui->m_filterEdit, SIGNAL(textChanged(QString)), this, SLOT(filterChanged(QString)) );
    connect(ui->m_iseView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentIndexChanged(QModelIndex)) );
    connect(ui->m_refreshButton, SIGNAL(clicked()), this, SLOT(refresh()) );

    refresh();
}

IseForm::~IseForm()
{
    delete ui;
}

void IseForm::filterChanged(const QString &_filter)
{
    m_proxyModel->setFilterFixedString(_filter);
}

void IseForm::currentIndexChanged(const QModelIndex &_index)
{
    if (_index.isValid())
    {
       m_contoller->loadModel(_index.data(IR_DATA).value<Model>());
    }
}

void IseForm::refresh()
{
    QList<Model> p_models = Db::selectIseModels();
    m_model->setModels(p_models);
}
