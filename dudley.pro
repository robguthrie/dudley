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
    state.cpp \
    statelogger.cpp \
    localdiskrepo.cpp \
    httpserver.cpp \
    mainwindow.cpp \
    repodialog.cpp \
    repomodel.cpp \
    httprequest.cpp \
    httpresponse.cpp \
    mimetypefinder.cpp \
    httpmessage.cpp \
    repo.cpp \
    httpcontroller.cpp \
    httpview.cpp \
    stateop.cpp \
    statediff.cpp \
    filetransfer.cpp \
    filetransfermanager.cpp \
    synchronizer.cpp

# filetransfer.cpp
HEADERS += fileinfo.h \
    state.h \
    statelogger.h \
    localdiskrepo.h \
    httpserver.h \
    mainwindow.h \
    repodialog.h \
    repomodel.h \
    httprequest.h \
    httpresponse.h \
    mimetypefinder.h \
    httpmessage.h \
    repo.h \
    httpcontroller.h \
    httpview.h \
    stateop.h \
    statediff.h \
    filetransfer.h \
    filetransfermanager.h \
    synchronizer.h

# filetransfer.h
# filerepomanager.h
FORMS += mainwindow.ui \
    repodialog.ui
OTHER_FILES += notes.txt \
    uploadform.html \
    apinotes.txt
RESOURCES += icon.qrc

unix:!symbian: LIBS += -L$$PWD/../qjson/build/lib/ -lqjson

INCLUDEPATH += $$PWD/../qjson/src
DEPENDPATH += $$PWD/../qjson/src
