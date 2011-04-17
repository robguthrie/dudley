# -------------------------------------------------
# Project created by QtCreator 2010-03-01T13:29:30
# -------------------------------------------------
QT += gui \
    network
TARGET = dudley
CONFIG += console
CONFIG -= app_bundle
LIBS += -L/Users/rob/src/qjson/src -lqjson
INCLUDEPATH = qjson
TEMPLATE = app
SOURCES += main.cpp \
    fileinfo.cpp \
    state.cpp \
    statelogger.cpp \
    localdiskrepo.cpp \
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
    httpcontroller.cpp \
    httpview.cpp \
    stateop.cpp

# filetransfer.cpp
HEADERS += fileinfo.h \
    state.h \
    statelogger.h \
    localdiskrepo.h \
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
    httpview.h \
    stateop.h

# filetransfer.h
# filerepomanager.h
FORMS += mainwindow.ui \
    repodialog.ui
OTHER_FILES += notes.txt \
    uploadform.html \
    apinotes.txt
RESOURCES += icon.qrc
