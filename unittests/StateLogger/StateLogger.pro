#-------------------------------------------------
#
# Project created by QtCreator 2011-01-29T21:46:25
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_stateloggertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -L/home/rob/src/qjson/lib -lqjson
INCLUDEPATH = ../.. /home/rob/src/qjson/src


SOURCES +=  ../../statelogger.cpp \
            ../../statediffop.cpp \
            ../../statediff.cpp \
            ../../state.cpp

HEADERS += ../../statelogger.h \
           ../../statediffop.h \
           ../../statediff.h \
           ../../state.h

SOURCES += tst_stateloggertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
