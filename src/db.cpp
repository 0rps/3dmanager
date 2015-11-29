#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QBuffer>

#include <QDebug>

#include <dbthread/dbthread.h>

#include "log.h"

#define DB_PATH "D:\\statdb.sqlite"

QSqlDatabase Db::m_statdb = QSqlDatabase();

static QString createDatabaseQuery(
        "CREATE TABLE models("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "united_id INTEGER,"
        "is_bug INTEGER,"
        "is_show INTEGER, "
        "army_id INTEGER,"
        "image BLOB,"
        "max BLOB,"
        "trids BLOB,"
        "texture BLOB);"
        "CREATE TABLE army("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "type_id INTEGER,"
        "is_russian INTEGER);"
        );

QString Db::m_dbpath("D:\\statdb.sqlite");

QList<Model> Db::selectIseModels()
{
    QString p_str = "select p_provide_el_united_id, p_provide_el_united_name "
            " from f_provide_el_photo_3d_select()";

    QSqlQuery p_query(dbthread::DbThread::instance()->dbForCurrentThread());
    p_query.prepare(p_str);

    QList<Model> p_models;
    if (p_query.exec())
    {
        while(p_query.next())
        {
            Model p_model;
            p_model.setUnitedId( p_query.value(0).toLongLong() );
            p_model.setName( p_query.value(1).toString() );
            p_models.append(p_model);
        }
    } else
        logError() << "selectIseModels. error=" << p_query.lastError().text();


    return p_models;
}

QByteArray Db::readTextureFromIse(const long _unitedId)
{
    QFile p_file(QString("file:///:3dstexture:%1").arg(_unitedId));
    if (p_file.open(QFile::ReadOnly))
        return p_file.readAll();
    else
        return QByteArray();
}

QByteArray Db::readModelFromIse(const long _unitedId)
{
    QFile p_file(QString("file:///:3dsmodel:%1").arg(_unitedId));
    if (p_file.open(QFile::ReadOnly))
        return p_file.readAll();
    else
        return QByteArray();
}

QList<long> Db::selectUnitedIdFromStat()
{
    QString p_strQuery = "SELECT united_id FROM models WHERE united_id > 0";
    QSqlQuery p_query(getStatDB());
    QList<long> p_result;
    if (p_query.exec(p_strQuery))
    {
        while (p_query.next())
        {
            p_result << p_query.value(0).toLongLong();
        }
    } else
        logError() << "selectUnitedIdFromStat. error = " << p_query.lastError().text();

    return p_result;
}

QList<Model> Db::selectModelsFromStat()
{
    const QString p_strQuery("SELECT id, name, united_id, is_bug, "
                             "army_id, image, max, trids, texture, is_show "
                             "FROM models");

    const int iid = 0;
    const int iname = 1;
    const int iunited = 2;
    const int iis_bug = 3;
    const int iarmy_id = 4;
    const int iimage = 5;
    const int imax = 6;
    const int itrids = 7;
    const int itexture = 8;
    const int ishow = 9;

    QList<Model> p_result;

    QSqlQuery p_query( getStatDB() );
    if ( p_query.exec(p_strQuery) )
    {
        Model p_model;

        while (p_query.next())
        {
            p_model.setId( p_query.value(iid).toString());
            p_model.setName( p_query.value(iname).toString() );
            p_model.setUnitedId(p_query.value(iunited).toLongLong() );
            p_model.setBug(p_query.value(iis_bug).toInt() > 0 );
            p_model.setArmy(p_query.value(iarmy_id).toInt() );
            p_model.setImage(p_query.value(iimage).toByteArray() );
            p_model.setMax(p_query.value(imax).toByteArray() );
            p_model.set3ds(p_query.value(itrids).toByteArray() );
            p_model.setTexture(p_query.value(itexture).toByteArray() );
            p_model.setShow(p_query.value(ishow).toInt() > 0);

            p_result << p_model;
        }
    } else
        logError() << "selectModelsFromStat." << p_query.lastError().text();

    return p_result;
}

bool Db::selectModelFromStat(Model &_model, const int _id)
{
    const QString p_strQuery("SELECT id, name, united_id, is_bug, "
                             "army_id, image, max, trids, texture, is_show "
                             "FROM models where id =:id");

    const int iid = 0;
    const int iname = 1;
    const int iunited = 2;
    const int iis_bug = 3;
    const int iarmy_id = 4;
    const int iimage = 5;
    const int imax = 6;
    const int itrids = 7;
    const int itexture = 8;
    const int ishow = 9;

    QSqlQuery p_query( getStatDB() );
    p_query.prepare(p_strQuery);
    p_query.bindValue(":id", _id);
    if ( p_query.exec() )
    {
        if (p_query.next())
        {
            _model.setId(_id);
            _model.setName( p_query.value(iname).toString() );
            _model.setUnitedId(p_query.value(iunited).toLongLong() );
            _model.setBug(p_query.value(iis_bug).toInt() > 0 );
            _model.setArmy(p_query.value(iarmy_id).toInt() );
            _model.setImage(p_query.value(iimage).toByteArray() );
            _model.setMax(p_query.value(imax).toByteArray() );
            _model.set3ds(p_query.value(itrids).toByteArray() );
            _model.setTexture(p_query.value(itexture).toByteArray() );
            _model.setShow(p_query.value(ishow).toInt() > 0);

            return true;
        } else
        {
            logInfo() << "No model in models with id=" << _id;
        }
    } else
        logError() << "selectModelFromStat." << p_query.lastError().text();

    return false;
}

bool Db::removeModelFromStat(const Model &_model)
{
    const QString p_strQuery = QString("DELETE models WHERE id = :id");
    QSqlQuery p_query( getStatDB() );
    p_query.prepare(p_strQuery);
    p_query.bindValue(_model.id());

    if (p_query.exec())
    {
        return true;
    }

    return false;
}

bool Db::updateModelInStat(const Model &_model)
{
    QString p_strQuery("UPDATE models SET name = :name,"
                       "united_id = :united_id, is_bug = :is_bug, "
                       "army_id = :army_id, "
                       "image = :image, max = :max, "
                       "trids = :trids, texture = :texture, is_show = :is_show "
                       "where id=:id");

    QSqlQuery p_query( getStatDB() );
    p_query.prepare( p_strQuery );
    p_query.bindValue(":id", _model.id());
    p_query.bindValue(":name", _model.name());
    p_query.bindValue(":united_id", _model.unitedId());
    p_query.bindValue(":is_bug", _model.isBug() ? 1 : 0);
    p_query.bindValue(":army_id", _model.armyId());
    p_query.bindValue(":image", _model.imageData());
    p_query.bindValue(":max", _model.max());
    p_query.bindValue(":trids", _model.trids());
    p_query.bindValue(":texture", _model.texture());
    p_query.bindValue(":is_show", _model.isShow()? 1:0);

    if ( false ==  p_query.exec() )
    {
        logError() << "updateModeINStat." << p_query.lastError().text();
        return false;
    }

    return true;
}

int Db::addModelsToStatDb(const QList<Model> &_models)
{
    int count = _models.size();
    foreach(const Model &_model, _models)
    {
        if ( false == addModelToStatDb(_model) )
        {
            count--;
        }
    }

    return count;
}

bool Db::addModelToStatDb(const Model &_model)
{
    QString p_strQuery("INSERT INTO models(name, united_id, is_bug, army_id, "
                       "image, max, trids, texture) VALUES( "
                       ":name, :united_id, :is_bug, :army_id,"
                       ":image, :max, :trids, :texture, :is_show);");

    QSqlQuery p_query( getStatDB() );
    p_query.prepare( p_strQuery );
    p_query.bindValue(":name", _model.name());
    p_query.bindValue(":united_id", _model.unitedId());
    p_query.bindValue(":is_bug", _model.isBug() ? 1 : 0);
    p_query.bindValue(":army_id", _model.armyId());
    p_query.bindValue(":image", _model.imageData());
    p_query.bindValue(":max", _model.max());
    p_query.bindValue(":trids", _model.trids());
    p_query.bindValue(":texture", _model.texture());
    p_query.bindValue(":is_show", _model.isShow()?1:0);

    if ( false ==  p_query.exec() )
    {
        logError() << "addModelToStat." << p_query.lastError().text();
        return false;
    }

    return true;

}

bool Db::updateArmy(const Army &_army)
{
    QString p_strQuery =
            "UPDATE army SET "
            "name = :name, "
            "type_id = :type_id, "
            "is_russian = :is_russian "
            "WHERE id = :id";

    QSqlQuery p_query( getStatDB() );
    p_query.prepare(p_strQuery);
    p_query.bindValue( ":name", _army.name() );
    p_query.bindValue( ":type_id", _army.type() );
    p_query.bindValue( ":is_russian", _army.isRussian() ? 1 : 0 );
    p_query.bindValue( ":id", _army.id() );

    if (false == p_query.exec())
    {
        logError() << "updateArmy." << p_query.lastError().text();
        return false;
    }

    return true;
}

bool Db::addArmy(const Army)
{
    QString p_strQuery =
            "INSERT INTO army( "
            "name, type_id, is_russian) VALUES( "
            ":name, :type_id, :is_russian)";

    QSqlQuery p_query( getStatDB() );
    p_query.prepare(p_strQuery);
    p_query.bindValue( ":name", _army.name() );
    p_query.bindValue( ":type_id", _army.type() );
    p_query.bindValue( ":is_russian", _army.isRussian() ? 1 : 0 );

    if (false == p_query.exec())
    {
        logError() << "addArmy." << p_query.lastError().text();
        return false;
    }

    return true;
}

QList<Army> Db::getArmies()
{
    QList<Army> p_result;
    QString p_strQuery =
            "SELECT id, name, type_id, is_russian FROM army";

    QSqlQuery p_query ( getStatDB() );
    p_query.prepare(p_strQuery);

    if ( false == p_query.exec() )
    {
        logError() << "getArmies." << p_query.lastError().text();
        p_result;
    }

    const int iid = 0;
    const int iname = 1;
    const int itype = 2;
    const int irussian = 3;

    Army p_army;
    while ( p_query.next() )
    {
        p_army.setId(_id);
        p_army.setName( p_query.value(iname).toString()  );
        p_army.setArmyType(p_query.value(itype).Int()  );
        p_army.setRussian(p_query.value(irussian).toInt() > 0 );

        p_result << p_army;
    }

    return p_result;
}

bool Db::getArmy(Army &_army, const int _id)
{
    QString p_strQuery =
            "SELECT id, name, type_id, is_russian "
            "FROM army where _id = :id";

    QSqlQuery p_query ( getStatDB() );
    p_query.prepare(p_strQuery);
    p_query.bindValue(":id", _id);

    if ( false == p_query.exec() )
    {
        logError() << "getArmy." << p_query.lastError().text();
        return false;
    }

    if (false == p_query.next())
    {
        logError() << "getArmy. no army with id = " << _id;
        return false;
    }

    const int iid = 0;
    const int iname = 1;
    const int itype = 2;
    const int irussian = 3;

    _army.setId(_id);
    _army.setName( p_query.value(iname).toString()  );
    _army.setArmyType(p_query.value(itype).Int()  );
    _army.setRussian(p_query.value(irussian).toInt() > 0 );

    return true;
}

void Db::setStatPath(const QString &_str)
{
    m_statdb.close();
    m_dbpath = _str;
}

QSqlDatabase Db::getStatDB()
{    
    if ( m_statdb.isOpen() )
        return m_statdb;

    bool p_exists = QFile::exists(m_dbpath) ;

    m_statdb = QSqlDatabase::addDatabase("QSQLITE","statdb");
    m_statdb.setDatabaseName(m_dbpath);
    m_statdb.open();

    if ( false == p_exists )
    {
        QSqlQuery p_query(m_statdb);
        p_query.prepare( createDatabaseQuery );
        if (false == p_query.exec())
        {
            qDebug() << "Error: " << p_query.lastError().text();
            terminate();
        }
    }
    return m_statdb;
}



Db::Db()
{
}
