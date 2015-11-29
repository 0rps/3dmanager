#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "edititem.h"

#include <syst/colorSelectionButton/systcolorselectionbutton.h>

#include "mainform.h"
#include "iseform.h"
#include "viewercontoller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_loader = new Viewer3dLoader(this);
    m_loader->setMode(EditItem::ModeEdit);
    QGLWidget *p_glWidget = dynamic_cast<QGLWidget*>(m_loader->createWidget());

    ViewerContoller *_controller = new ViewerContoller(p_glWidget, m_loader, this);

    MainForm *p_form = new MainForm();
    p_form->addWidget(new IseForm(_controller, p_form), "ISE");
    p_form->addWidget(new ModelsForm(_controller, p_form), "Add new");

    this->setCentralWidget(p_glWidget);
    resize(1920,1080);

    p_form->show();

    syst::SystColorSelectionButton p_button;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dropEvent(QDropEvent *_event)
{

}
