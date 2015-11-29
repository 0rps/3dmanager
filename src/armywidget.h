#ifndef ARMYWIDGET_H
#define ARMYWIDGET_H

#include <QWidget>

#include "army.h"

namespace Ui {
class ArmyWidget;
}

class ArmyWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ArmyWidget(const Army &_army, const bool _isEdit, QWidget *parent = 0);
    ~ArmyWidget();
    
    Army army() const;

private slots:
    void typeChanged(const int _index);
    void nameChanged(const QString &_name);
    void russianBoxChecked(const bool _isChecked);

private:
    void init(const bool _isEdit);

private:
    Ui::ArmyWidget *ui;

    Army m_army;
};

#endif // ARMYWIDGET_H
