#ifndef ARMYITEMMODEL_H
#define ARMYITEMMODEL_H

#include <QAbstractItemModel>

#include "army.h"

class ArmyItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum ModelRole
    {
        MR_TYPE = Qt::UserRole,
        MR_NAME
    };

    enum ModelColumn
    {
        MC_ID = 0,
        MC_NAME,
        MC_ISRUSSIAN,
        MC_TYPEID,
        MC_SIZE
    };

    explicit ArmyItemModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    QVariant data(const QModelIndex &index, int role) const;


signals:
    
public slots:
    void revert();

private:
    QList<Army> m_armies;
};

#endif // ARMYITEMMODEL_H
