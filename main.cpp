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
    QString config_path = collection_path+"/.dudley";
    QString logsDir = config_path + "/logs";
    QStringList params;
    for(i = 1; i < argc; ++i){
        params << argv[i];
    }
    if (params.size() == 0){

    }else{
        Logger *logger = new Logger(logsDir, true);
        FileInfoCollection *collection = new FileInfoCollection(collection_path);
        if (params[0] == "track"){
            bool read_sha1s = false;
            bool write_log = false;
            if (params.size() > 1){
                if (params[1] == "init"){
                    if (collection->create()){
                        std::cout << "initialized dudley history at " << qPrintable(m_path) << std::endl;
                    }else{
                        std::cout << "could not initalize dudley history at " << qPrintable(m_path) << std::endl;
                    }
                }else if (params[1] == "commit"){
                    read_sha1s = true;
                    write_log = true;
                }
            }

            if (collection->exists()){
                logger->playLogs(collection);
                collection->scanForChanges(collection_path, logger, read_sha1s);
                logger->printLogFile();
                if (write_log) logger->writeLogFile();
            }else{
                std::cout << "dudley history not found. run track init or cd to correct directory" << std::endl;
            }
        }else if (params[0] = "sync"){
            if (!collection->exists()){
                std::cout << "dudley history not found. run track init or cd to correct directory" << std::endl;
            }else{
                logger->playLogs(collection);
                if (params.size() > 1){
                    if (params[1] == "init"){
                        // and we have a valid path for the new collection
                        // create a storage collection
                    }else if (params[1] == "commit"){

                    }
                }else{
                    // just show sync suggestions
                    // pass the storage collection the fileinfocollection and see what needs to be added
                    // if files look to have been deleted.. they may be missing..
                }
            }

        }
    }



        if (command == "track"){
            bool read_sha1s = false;
            if (command1 == "commit"){
                read_sha1s = true;
                collection->scanForChanges(collection_path, logger, read_sha1s);
                logger->writeLogFile();
            }else{
                collection->scanForChanges(collection_path, logger, read_sha1s);
                logger->printLogFile();
            }
        } else if (command == "sync"){
            // instanciate a storage collection
            // and go about finding what needs to be synced
            StorageCollection store("/home/rob/dudley_store");
            // first want to find out what keys from the fileinfocollection of the local working dir
            // are not stored within the storage collection
            QStringList missing_keys  = store.listMissingKeys(collection.);

        } else if (command == "init"){
            std::cout << qPrintable(collection_path) << " is already a dudley working directory" << std::endl;
        } else {
            std::cout << "unknown command: " << qPrintable(command) << std::endl;
            std::cout << "try init, track, or sync" << qPrintable(command) << std::endl;
        }
    }

    return 0;
}

