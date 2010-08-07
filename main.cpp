#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <QSettings>

#include "filerepo.h"
#include "filerepostate.h"
#include "filerepostatelogger.h"
#include "workingfilerepo.h"
#include "output.h"
#include "mainwindow.h"

// my master backup is on the internet

int main(int argc, char *argv[])
{
    QStringList params;
    for(int i = 1; i < argc; ++i) params << argv[i];

    if (params.size() == 0){
        // gui mode here
        Output::error("default action undefined right now");
        Output::info("try dudley repo");
    }else if (params[0] == "repo"){
        QString collection_path = QDir::currentPath();
        // open working filerepo at our location
        WorkingFileRepo *repo = new WorkingFileRepo(0, collection_path);

        if ((params.size() > 1) && (params[1] == "init")){
            if (repo->initialize()){
                // success
                Output::info("Initialised WorkingFileRepo");
                return 0;
            }else{
                Output::error(QString("Failed to initialize WorkingFileRepo").append(collection_path));
                return 1;
            }
        }else if (repo->isReady()){
            Output::verbose("Opened WorkingFile Repo. Updating Known State");
            repo->updateState();
            repo->state()->logger()->printLogFile();
            // we save changes by default now..
            // if we were going to throw away all those fingerprints we shouldnt
            // do them in the first place
            repo->state()->saveChanges();
        }else{
            Output::warning("Could not open workingfile repo - prehaps init?");
        }
        //print any pending changes to the repo history
    }else if (params[0] == "server"){
        // really gui mode
        // within the mainwindow..
        // want to load the user system wide settings
        // get a list of repos and populate it
        // enable the server to use the repos and serve indexes and files
        QApplication app(argc, argv);
        QApplication::setOrganizationName("Dinotech");
        QApplication::setOrganizationDomain("dinotech.co.nz");
        QApplication::setApplicationName("Dudley");
        MainWindow mainWindow;
        mainWindow.show();
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        return app.exec();
    }
    return 0;
}
