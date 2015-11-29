#ifndef ISEMODEL_H
#define ISEMODEL_H

#include <QAbstractItemModel>

#include "globals.h"


enum IseRole
{
    IR_ID = Qt::UserRole,
    IR_NAME,
    IR_DATA
};

enum IseColumn
{
    IC_NAME = 0,
    IC_SIZE
};

class ModelModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ModelModel(QObject *parent = 0);
    ~ModelModel() {}
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    bool removeRows(int row, int count, const QModelIndex &parent);
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QList<Model> getModels() const;

public slots:
    void clear();
    void addModel(const Model &_model);
    void setModels(const QList<Model> &_list);

private:
    QList<Model> m_models;

};

#endif // ISEMODEL_H
