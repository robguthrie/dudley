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
    repostate.cpp \
    repostatelogger.cpp \
    localdiskrepo.cpp \
    localstoragerepo.cpp \
    output.cpp \
    httpserver.cpp \
    mainwindow.cpp \
    repodialog.cpp \
    repomodel.cpp \
    httprequest.cpp \
    httpresponse.cpp \
    httpclientrepo.cpp \
    mimetypefinder.cpp \
    filetransfer.cpp \
    filetransfermanager.cpp \
    httpmessage.cpp \
    repo.cpp \
    readoncebuffer.cpp \
    httpcontroller.cpp \
    httpview.cpp

# filetransfer.cpp
HEADERS += fileinfo.h \
    repostate.h \
    repostatelogger.h \
    localdiskrepo.h \
    localstoragerepo.h \
    output.h \
    httpserver.h \
    mainwindow.h \
    repodialog.h \
    repomodel.h \
    httprequest.h \
    httpresponse.h \
    httpclientrepo.h \
    mimetypefinder.h \
    filetransfer.h \
    filetransfermanager.h \
    httpmessage.h \
    repo.h \
    readoncebuffer.h \
    httpcontroller.h \
    httpview.h

# filetransfer.h
# filerepomanager.h
FORMS += mainwindow.ui \
    repodialog.ui
OTHER_FILES += notes.txt \
    uploadform.html \
    apinotes.txt
RESOURCES += icon.qrc
