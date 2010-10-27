#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <QSettings>

#include "repo.h"
#include "repostate.h"
#include "repostatelogger.h"
#include "localdiskrepo.h"
#include "output.h"
#include "mainwindow.h"

Output* g_log = 0;
// my master backup is on the internet
int main(int argc, char *argv[])
{
    g_log = new Output();
    QStringList params;
    for(int i = 1; i < argc; ++i) params << argv[i];
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    QApplication::setOrganizationName("Dinotech");
    QApplication::setOrganizationDomain("dinotech.co.nz");
    QApplication::setApplicationName("Dudley");

    QSettings* settings;
    if (args.size() > 1){
        settings = new QSettings(args[1]);
    }else{
        settings = new QSettings();
    }
    MainWindow mainWindow(settings);
    mainWindow.show();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return app.exec();
}
