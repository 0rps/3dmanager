#ifndef DB_H
#define DB_H

#include <QObject>

#include <QSqlDatabase>

#include "globals.h"

class Db
{
public:
    static QList<Model> selectIseModels();
    static QByteArray readTextureFromIse(const long _unitedId);
    static QByteArray readModelFromIse(const long _unitedId);

    static QList<long> selectUnitedIdFromStat();
    static QList<Model> selectModelsFromStat();
    static bool selectModelFromStat(Model &_model, const int _id);
    static bool removeModelFromStat(const Model &_model);
    static bool updateModelInStat(const Model &_model);
    static int addModelsToStatDb(const QList<Model> &_models);
    static bool addModelToStatDb(const Model &_model);

    static bool addArmy(const Army &_army);
    static bool updateArmy(const Army &_army);
    static QList<Army> getArmies();
    static bool getArmy(Army &_army, const int _id);

    static void setStatPath(const QString &_str);

protected:
    static QSqlDatabase getStatDB();
    static QString m_dbpath;
    explicit Db();
private:
    static QSqlDatabase m_statdb;

};

#endif // DB_H
