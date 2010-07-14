#include <QtCore/QCoreApplication>
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
// my master backup is on the internet

using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Dinotech");
    QCoreApplication::setOrganizationDomain("dinotech.co.nz");
    QCoreApplication::setApplicationName("Dudley");
    QSettings settings; // i know we are going to need it


    QStringList params;
    for(int i = 1; i < argc; ++i) params << argv[i];

    if (params.size() == 0){
        // gui mode here
        Output::error("default action undefined right now");
        Output::info("try dudley track");
    }else{
        if (params[0] == "repo"){
            QString collection_path = QDir::currentPath();
            // open working filerepo at our location
            WorkingFileRepo *repo = new WorkingFileRepo(collection_path);

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

                repo->updateState();
                repo->state()->logger()->printLogFile();
                if ((params.size() > 1) && (params[1] == "commit")){
                    repo->state()->saveChanges();
                }
            }else{
                Output::warning("why is this?");

            }
            //print any pending changes to the repo history
        }
    }
    return 0;
}
