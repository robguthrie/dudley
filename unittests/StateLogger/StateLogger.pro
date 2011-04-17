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

SOURCES +=  ../../statelogger.cpp \
            ../../stateop.cpp \
            ../../state.cpp \
            ../../fileinfo.cpp \
            ../../mimetypefinder.cpp

HEADERS += ../../statelogger.h \
           ../../stateop.h \
           ../../state.h \
           ../../fileinfo.h \
           ../../mimetypefinder.h

SOURCES += tst_stateloggertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

unix:!symbian: LIBS += -L$$PWD/../../../qjson/build/lib/ -lqjson

INCLUDEPATH += $$PWD/../../../qjson/src
DEPENDPATH += $$PWD/../../../qjson/src
