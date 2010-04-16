#include <QtCore/QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <QSettings>

#include "fileinfocollection.h"
#include "storagecollection.h"
#include "logger.h"

// my master backup is on the internet

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString collection_path = QDir::currentPath();
    QStringList params;
    for(int i = 1; i < argc; ++i) params << argv[i];

    if (params.size() == 0){
        std::cout << "default action undefined right now" << std::endl;
    }else{
        Logger *logger = new Logger(collection_path, false);
        if (params[0] == "track"){
            if ((params.size() > 1) && (params[1] == "init")){
                if (logger->initialize()){
                    std::cout << "initialized history log within " << qPrintable(collection_path) << std::endl;
                }else{
                    std::cout << "failed to initialize history log within " << qPrintable(collection_path) << std::endl;
                }
            }else{

                if (logger->isInitialized()){
                    logger->playLogs();
                    logger->stageWorkingDirectory();
                    logger->printLogFile();
                    // print some statistics about the repo
                    if ((params.size() > 1) && (params[1] == "commit")){
                        logger->writeLogFile();
                    }
                }else{
                    std::cout << "dudley history not found. run track init or cd to correct directory" << std::endl;
                }
            }
        } else if (params[0] == "sync"){
            // want to know a transfer list required to make the working directory
            // represent the fileinfocollection in full
            // which files are missing within the working dir?
            // which files are missing from the storage collection?
            if (logger->isInitialized()){
                StorageCollection* storage = new StorageCollection("/home/rob/dudley_store");
                storage->scanCollection();

                // need to know files mentioned in the fileinfocollection but missing in the working directory
                QStringList* found_files = logger->filesOnDisk();
                QHash<QString, FileInfo*> missing_files = logger->collection()->missingFiles(found_files);
                FileInfo* file_info;
                // so iterate over the fileinfos and ask the storage if it has the files we are looking for..
                foreach(file_info, missing_files){
                    if (storage->contains(file_info->fingerPrint())){
                        // add to the queue of stuff we would copy from storage to working
                    }else{
                        // add to the list of stuff we have no copies of
                    }
                }

                // wanna know what stuff, that we have locally, the storage does not have.
                // so known_found_files..
                QHash<QString, FileInfo*> known_found_files = logger->collection()->knownFiles(found_files);
                foreach(file_info, known_found_files){
                    if (storage->contains(file_info->fingerPrint())){
                        // it is in storage already
                    }else{
                        // add it to the queue of stuff to put in storage
                    }
                }
            }
        } else {
            std::cout << "command unknown" << std::endl;
        }

    }




    return 0;
}

