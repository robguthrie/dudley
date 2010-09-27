#include <QtTest/QtTest>
#include "../../workingfilerepo.h"
class WorkingFileRepo : public QObject
{
    Q_OBJECT
private slots:
    void testWorkingFileRepoBasic();
};


void WorkingFileRepo::testWorkingFileRepoBasic(){
    // test creating, initializing, updating
//    QString collection_path = QDir::currentPath();
//    // open working filerepo at our location
//    WorkingFileRepo *repo = new WorkingFileRepo(0, collection_path, "commandline");
//
//    if ((params.size() > 1) && (params[1] == "init")){
//        if (repo->initialize()){
//            // success
//            Output::info("Initialised WorkingFileRepo");
//            return 0;
//        }else{
//            Output::error(QString("Failed to initialize WorkingFileRepo").append(collection_path));
//            return 1;
//        }
//    }else if (repo->isReady()){
//        Output::verbose("Opened WorkingFile Repo. Updating Known State");
//        repo->updateState();
//        repo->state()->logger()->printLogFile();
//        // we save changes by default now..
//        // if we were going to throw away all those fingerprints we shouldnt
//        // do them in the first place
//        repo->state()->saveChanges();
//    }else{
//        Output::warning("Could not open workingfile repo - prehaps init?");
//    }
}

QTEST_MAIN(TestWorkingFileRepo)
#include "testworkingfilerepo.moc"

