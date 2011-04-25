#-------------------------------------------------
#
# Project created by QtCreator 2011-04-21T09:32:07
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_localdiskrepotest
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
            tst_localdiskrepotest.cpp

HEADERS += ../../statelogger.h \
           ../../statediff.h \
           ../../stateop.h \
           ../../state.h \
           ../../fileinfo.h \
           ../../mimetypefinder.h \
           ../../repo.h \
           ../../localdiskrepo.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"

unix:!symbian: LIBS += -L$$PWD/../../../qjson/build/lib/ -lqjson

INCLUDEPATH += $$PWD/../../../qjson/src
DEPENDPATH += $$PWD/../../../qjson/src
