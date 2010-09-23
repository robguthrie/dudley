# -------------------------------------------------
# Project created by QtCreator 2010-03-01T13:29:30
# -------------------------------------------------
QT += gui \
    network \
    xml
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
    mainwindow.cpp \
    repodialog.cpp \
    repotablemodel.cpp \
    httprequest.cpp \
    httpresponse.cpp \
    mime/freedesktopmime.cpp \
    httpclientfilerepo.cpp \
    filetransfer.cpp
HEADERS += fileinfo.h \
    filerepostate.h \
    filerepostatelogger.h \
    filerepo.h \
    workingfilerepo.h \
    storagefilerepo.h \
    output.h \
    server.h \
    mainwindow.h \
    repodialog.h \
    repotablemodel.h \
    httprequest.h \
    httpresponse.h \
    mime/freedesktopmime.h \
    httpclientfilerepo.h \
    filetransfer.h

# filerepomanager.h
FORMS += mainwindow.ui \
    repodialog.ui
OTHER_FILES += 
RESOURCES += icon.qrc \
    mime\freedesktopmime.qrc \
    mime/freedesktopmime.qrc
