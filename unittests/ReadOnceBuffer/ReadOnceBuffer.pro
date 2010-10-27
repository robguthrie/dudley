#-------------------------------------------------
#
# Project created by QtCreator 2010-10-26T17:53:15
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_readoncebuffertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../../readoncebuffer.h\
           ../../output.h
SOURCES += tst_readoncebuffertest.cpp\
../../readoncebuffer.cpp\
../../output.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
