#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../localdiskrepo.h"
#include "../../synchronizer.h"
class SynchronizerTest : public QObject
{
    Q_OBJECT

public:
    SynchronizerTest();

private Q_SLOTS:
    void testCase1();
private:
    QString setupEmptyDir(QString name) const;
    QString setupDirWithFiles(QString name) const;
    QString startTracker() const;
    void waitAndListen(int secs) const;
    int run(QString command) const;

};

SynchronizerTest::SynchronizerTest()
{
}


QString SynchronizerTest::setupEmptyDir(QString name = "test_repo") const
{
    QStringList commands;
    QString dir = QDir::currentPath()+"/"+name;
    commands << QString("rm -r %1").arg(dir);
    commands << QString("mkdir %1").arg(dir);
    foreach(QString command, commands){
        run(command);
    }
    return dir;
}

QString SynchronizerTest::setupDirWithFiles(QString name = "test_repo") const
{
    QString dir = setupEmptyDir(name);
    QString file1_path = dir+"/file1.txt";
    QString file2_path = dir+"/file2.txt";

    QStringList commands;
    commands << QString("echo \"something like the contents of a file\" > %1").arg(file1_path);
    commands << QString("echo \"another file with a wonderful message\" > %1").arg(file2_path);
    foreach(QString command, commands){
        run(command);
    }
    return dir;
}

QString SynchronizerTest::startTracker() const
{
    run("/usr/local/bin/mysql -uroot dudley_tracker_test < ../sqldumps/empty_state.sql");
    //    run("cd ../../../dudley_tracker/ && echo $PWD && RAILS_ENV=test /Users/rob/.rvm/gems/ruby-1.9.2-p180/bin/rails server -p 3456 &");
    //    QTest::qWait(4000);
    return QString("http://localhost:3456/states/1");
}

int SynchronizerTest::run(QString command) const
{
    qDebug() << command;
    return system(command.toAscii().data());
}

void SynchronizerTest::waitAndListen(int secs = 1) const
{
    for(int i = 0; i < secs; i++){
        QTest::qWait(1000);
        QCoreApplication::processEvents();
    }
}

/* testing procedure
    sync, and verify push of changes
    sync again and verify synced (no changes to push)

    verify
    pushing statediffs
        load up empty but created state
        create smallbutsignificant repo on disk, detect and log changes

    pulling statediffs from server
        load up "smallbutsignificant state into test db"
        create empty repo and sync it against corresponding url
        verify that we have all statediffs from sbs state


    not doing anything when both sides have everything

   verify that both have no log files yet
   detectChanges on local repo and accept them
   push these changes to the tracker
   verify that the tracker has the changes
   verify that everything is uptodate */
void SynchronizerTest::testCase1()
{
    /*
       start tracker in background
       create localdir with files
       scan, accept changes, init synchronizer
       */
    qDebug() << QDir::currentPath();
    QString tracker_url = startTracker();
    QString local_path = setupDirWithFiles();
    LocalDiskRepo ldr(0, local_path);
    ldr.initialize();
    ldr.detectChanges();
    ldr.logger()->acceptChanges();
    Synchronizer sync(0, &ldr, tracker_url);
    QVERIFY2(sync.isReady(), "Synchronizer is not ready");
    QVERIFY2(!sync.isActive(), "Synchronizer should not be looping");

    /*
       request history, wait for response
       verify that we have statediffs to push, none to pull
     */
    sync.requestHistory();
    waitAndListen();

    QVERIFY2(sync.stateDiffsToPush().count() == 1, "incorrect number of statediffs to push");
    QVERIFY2(sync.stateDiffsToPull().count() == 0, "incorrect number of statediffs to pull");

    /*
      if we run the loop, and wait for the responses to come through
      then verify that we now have nothing to push
      */
    sync.loop();
    waitAndListen();
    sync.loop();
    sync.requestHistory();
    waitAndListen();

    QVERIFY2(sync.stateDiffsToPush().count() == 0, "incorrect number of statediffs to push after loop");
    QVERIFY2(sync.stateDiffsToPull().count() == 0, "incorrect number of statediffs to pull affer loop");

    /*
      ok so now we should be able to sync a empty repo to the url and get the state diffs
      that we pushed earlier. lets try
      */

    LocalDiskRepo nldr(0, setupEmptyDir("pull_repo"));
    nldr.initialize();
    nldr.detectChanges();
    QVERIFY2(nldr.logger()->stateDiff()->numChanges() == 0, "new localdiskrepo should not have changes");
    Synchronizer sync2(0, &nldr, tracker_url);
    QVERIFY2(sync2.isReady(), "new syncer is not ready");
    sync2.requestHistory();
    waitAndListen();
    QVERIFY2(sync2.stateDiffsToPull().count() == 1, "should have a statediff to pull");
    sync2.loop();
    waitAndListen();
}

QTEST_MAIN(SynchronizerTest);

#include "tst_synchronizertest.moc"
