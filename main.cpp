#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <QSettings>

#include "repo.h"
#include "state.h"
#include "statelogger.h"
#include "localdiskrepo.h"
#include "output.h"
#include "mainwindow.h"

// my master backup is on the internet
int main(int argc, char *argv[])
{
    QStringList params;
    for(int i = 1; i < argc; ++i) params << argv[i];
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    QApplication::setOrganizationName("Dinotech");
    QApplication::setOrganizationDomain("dinotech.co.nz");
    QApplication::setApplicationName("Dudley");

    MainWindow mainWindow;
    mainWindow.show();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return app.exec();
}
