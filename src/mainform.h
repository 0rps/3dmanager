#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

    void addWidget(QWidget *_widget, const QString &_name);

private:
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
