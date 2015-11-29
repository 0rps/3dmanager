#-------------------------------------------------
#
# Project created by QtCreator 2015-09-15T19:38:46
#
#-------------------------------------------------

QT       += core gui opengl sql

LIBS += -l3ds

LIBS                +=  -ldbthread \
                        -ludesksettings \
                        -lsyst

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ./src ../include

TARGET = 3dmanager
TEMPLATE = app
DESTDIR = app

include(../include/priutils/debugandrelease.pri)

include(3dviewer.pri)
include(sources.pri)

