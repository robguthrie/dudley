#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T10:08:52
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_upnpwrappertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_upnpwrappertest.cpp \
    ../../upnpwrapper.cpp
INCLUDEPATH += $$PWD/../../../miniupnpc
DEFINES += SRCDIR=\\\"$$PWD/\\\"
LIBS += -lminiupnpc

HEADERS += \
    ../../upnpwrapper.h
