#-------------------------------------------------
#
# Project created by QtCreator 2011-01-16T20:33:04
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_statedifftest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -L/home/rob/src/qjson/lib -lqjson
INCLUDEPATH = ../.. /home/rob/src/qjson/src


SOURCES += tst_statedifftest.cpp \
            ../../statediff.cpp \
            ../../statediffop.cpp
HEADERS += ../../statediff.h \
           ../../statediffop.h
DEFINES += SRCDIR=\\\"$$PWD/\\\"
