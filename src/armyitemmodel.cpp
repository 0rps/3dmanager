#include "armyitemmodel.h"

#include "db.h"

ArmyItemModel::ArmyItemModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    revert();
}

int ArmyItemModel::rowCount(const QModelIndex &parent) const
{
    return m_armies.size();
}

int ArmyItemModel::columnCount(const QModelIndex &parent) const
{
    return MC_SIZE;
}

QModelIndex ArmyItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row >= 0 && column >= 0 && row < rowCount(parent) && column < columnCount(parent))
    {
        createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex ArmyItemModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QVariant ArmyItemModel::data(const QModelIndex &index, int role) const
{
    if ( false == index.isValid() )
        return;

    Army p_army = m_armies.at(index.row());


    if (Qt::DisplayRole == role)
    {
        switch (index.column())
        {
        case MC_ID:
            return p_army.id();
        case MC_ISRUSSIAN:
            return p_army.isRussian();
        case MC_NAME:
            return p_army.name();
        case MC_TYPEID:
            return p_army.type();

        default:
            break;
        }
    } else if (MR_NAME == role)
    {
        return p_army.name();
    }
    else if (MR_TYPE == role)
    {
        return p_army.type();
    }

    return QVariant();
}

void ArmyItemModel::revert()
{
    modelAboutToBeReset();
    m_armies = Db::getArmies();
    modelReset();
}
