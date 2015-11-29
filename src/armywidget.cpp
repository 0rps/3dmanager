#include "armywidget.h"
#include "ui_armywidget.h"

enum INTERNAL_ROLE
{
    IR_TYPEID = Qt::UserRole
};

ArmyWidget::ArmyWidget(const Army &_army, const bool _isEdit, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArmyWidget),
    m_army(_army)
{
    ui->setupUi(this);

    init(_isEdit);

    connect(ui->m_isRussianCheck, SIGNAL(clicked(bool)), this, SLOT(russianBoxChecked(bool)) );
    connect(ui->m_nameEdit, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));
    connect(ui->m_internalTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)) );
}

ArmyWidget::~ArmyWidget()
{
    delete ui;
}

Army ArmyWidget::army() const
{
    return m_army;
}

void ArmyWidget::typeChanged(const int _index)
{
    m_army.setArmyType( ui->m_internalTypeBox->itemData(_index, IR_TYPEID).toInt() );
}

void ArmyWidget::nameChanged(const QString &_name)
{
    m_army.setName(_name);
}

void ArmyWidget::russianBoxChecked(const bool _isChecked)
{
    m_army.setRussian(_isChecked);
}

void ArmyWidget::init(const bool _isEdit)
{
    ui->m_isRussianCheck->setEnabled(_isEdit);
    ui->m_nameEdit->setReadOnly(!_isEdit);
    ui->m_internalTypeBox->setEnabled(_isEdit);

    ui->m_isRussianCheck->setChecked( m_army.isRussian() );
}
