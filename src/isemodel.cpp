#include <QtSql/QSqlQuery>

#include <udesksettings/librarysettings.h>
#include <macro/log.h>

#include "isemodel.h"

#include "db.h"

ModelModel::ModelModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

int ModelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_models.size();
}

int ModelModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return IC_SIZE;
}

QVariant ModelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( Qt::Horizontal == orientation && Qt::DisplayRole == role )
    {
        switch(section){
            case IC_NAME:
                return QVariant("Название");
        }
    }

    return QVariant();
}

QList<Model> ModelModel::getModels() const
{
    return m_models;
}

void ModelModel::clear()
{
    beginResetModel();
    m_models.clear();
    endResetModel();
}

void ModelModel::addModel(const Model &_model)
{
    beginInsertRows(QModelIndex(), m_models.count(), m_models.count());
    m_models.append(_model);
    endInsertRows();
}

void ModelModel::setModels(const QList<Model> &_list)
{
    beginResetModel();
    m_models = _list;
    endResetModel();
}

QModelIndex ModelModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

bool ModelModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, count + row - 1);
    while (count > 0)
    {
        m_models.removeAt(row);
        row++;
        count--;
    }
    endRemoveRows();
    return true;
}

QModelIndex ModelModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return createIndex(row, column);
}

QVariant ModelModel::data(const QModelIndex &index, int role) const
{
    if ( Qt::DisplayRole == role )
    {
        Model pModel = m_models.at(index.row());
        switch (index.column()) {
        case IC_NAME:
            return pModel.name;
            break;
        default:
            break;
        }
    }

    if ( IR_ID == role )
    {
        return m_models.at(index.row()).united_id;
    } else if ( IR_NAME == role )
    {
        return m_models.at(index.row()).name;
    } else if ( IR_DATA == role )
    {
        return QVariant::fromValue(m_models.at(index.row()));
    }

    return QVariant();
}


