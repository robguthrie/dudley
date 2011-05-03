#-------------------------------------------------
#
# Project created by QtCreator 2011-04-28T22:03:37
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = tst_synchronizertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES +=  ../../statelogger.cpp \
            ../../statediff.cpp \
            ../../stateop.cpp \
            ../../state.cpp \
            ../../fileinfo.cpp \
            ../../mimetypefinder.cpp \
            ../../repo.cpp \
            ../../localdiskrepo.cpp \
            ../../synchronizer.cpp

HEADERS += ../../statelogger.h \
           ../../statediff.h \
           ../../stateop.h \
           ../../state.h \
           ../../fileinfo.h \
           ../../mimetypefinder.h \
           ../../repo.h \
           ../../localdiskrepo.h \
           ../../synchronizer.h

unix:!symbian: LIBS += -L$$PWD/../../../qjson/build/lib/ -lqjson

INCLUDEPATH += $$PWD/../../../qjson/src
DEPENDPATH += $$PWD/../../../qjson/src

SOURCES += tst_synchronizertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
