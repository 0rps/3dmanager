#include "mainwindow.h"
#include <QApplication>

#include <QTextCodec>
#include <QSettings>
#include <QSqlDatabase>
#include <QFile>

bool openDb() {

    if (false == QFile::exists("settings/dbsettings.ini"))
        return false;

    QSettings settings("settings/dbsettings.ini", QSettings::IniFormat);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName( settings.value("Database_Conection/Host").toString() );
    db.setDatabaseName( settings.value("Database_Conection/Database").toString() );
    db.setUserName( settings.value("Database_Conection/User").toString() );
    db.setPassword( settings.value("Database_Conection/Password").toString() );
    db.setPort( settings.value("Database_Conection/Port", "5432").toInt() );

    return db.open();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP1251"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));

    if (false == openDb())
    {
        terminate();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
