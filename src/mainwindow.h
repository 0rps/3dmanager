#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDropEvent>

#include "viewer3dloader.h"
#include "modelsform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void dropEvent(QDropEvent *_event);

private:
    Ui::MainWindow *ui;
    Viewer3dLoader *m_loader;

};

#endif // MAINWINDOW_H
