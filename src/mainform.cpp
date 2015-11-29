#include "mainform.h"
#include "ui_mainform.h"

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::addWidget(QWidget *_widget, const QString &_name)
{
    ui->m_tabwidget->addTab(_widget, _name);
}
