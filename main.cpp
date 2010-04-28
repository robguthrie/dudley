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

// my master backup is on the internet
using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString collection_path = QDir::currentPath();
    QString logs_dir = QDir::currentPath() + "/.dudley/logs";
    cout << "logs_dir:" << qPrintable(logs_dir) << endl;
    QStringList params;
    for(int i = 1; i < argc; ++i) params << argv[i];

    if (params.size() == 0){
        cout << "default action undefined right now" << endl;
    }else{
        // check that the working directory and logger are initialized
        if (params[0] == "track"){
            // start working filerepo at our location
            FileRepoStateLogger *logger = new FileRepoStateLogger(logs_dir);
            if ((params.size() > 1) && (params[1] == "init")){
                if (logger->initialize()){
                    cout << "initialized history log within " << qPrintable(collection_path) << endl;
                }else{
                    cerr << "failed to initialize history log within " << qPrintable(collection_path) << endl;
                    return 1;
                }
            }else{
                if (!logger->isInitialized()){
                    cerr << "histoy log not initialized. run 'track init'' or cd to correct directory:" << qPrintable(logs_dir) << endl;
                    return 1;
                }
                FileRepoState *state = new FileRepoState(logger);
                WorkingFileRepo *repo = new WorkingFileRepo(collection_path, state);
                repo->stageChanges();
                logger->printLogFile();
                // print some statistics about the repo
                if ((params.size() > 1) && (params[1] == "commit")){
                    logger->writeLogFile();
                }
            }
        }
    }
    return 0;
        /* else if (params[0] == "sync"){
            // want to know a transfer list required to make the working directory
            // represent the fileinfocollection in full
            // which files are missing within the working dir?
            // which files are missing from the storage collection?
            // QueueManager

            // list files in the fileinfocollection and not in storage
            // storageCollection->listMissingFiles(fileInfoCollection)
            // storageCollection->listStoredFiles(fileInfoCollection)
            // queueManager->upload(file to storage colleciton)
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
            cout << "command unknown" << endl;
        }*/
}

