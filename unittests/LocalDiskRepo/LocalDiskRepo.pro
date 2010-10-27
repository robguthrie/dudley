#-------------------------------------------------
#
# Project created by QtCreator 2010-10-26T12:19:24
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_localdiskrepotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../../localdiskrepo.h\
    ../../repo.h\
    ../../repostate.h\
    ../../repostatelogger.h\
    ../../fileinfo.h\
    ../../mimetypefinder.h\
    ../../output.h
SOURCES += tst_localdiskrepotest.cpp\
    ../../localdiskrepo.cpp\
    ../../repo.cpp\
    ../../repostate.cpp\
    ../../repostatelogger.cpp\
    ../../fileinfo.cpp\
    ../../output.cpp\
    ../../mimetypefinder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
