# -------------------------------------------------
# Project created by QtCreator 2010-03-01T13:29:30
# -------------------------------------------------
QT += gui \
    network
TARGET = dudley
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    fileinfo.cpp \
    filerepostate.cpp \
    filerepostatelogger.cpp \
    filerepo.cpp \
    workingfilerepo.cpp \
    storagefilerepo.cpp \
    output.cpp \
    server.cpp \
    mainwindow.cpp
HEADERS += fileinfo.h \
    filerepostate.h \
    filerepostatelogger.h \
    filerepo.h \
    workingfilerepo.h \
    storagefilerepo.h \
    output.h \
    server.h \
    mainwindow.h

# filerepomanager.h
FORMS += mainwindow.ui
