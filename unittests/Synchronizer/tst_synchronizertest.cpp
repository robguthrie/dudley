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
    QUrl startTracker() const;
    void waitAndListen(int secs) const;
    void run(QString command) const;

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

QUrl SynchronizerTest::startTracker() const
{
    run("/usr/local/bin/mysql -uroot dudley_tracker_test < ../sqldumps/empty_state.sql");
    //    run("cd ../../../dudley_tracker/ && echo $PWD && RAILS_ENV=test /Users/rob/.rvm/gems/ruby-1.9.2-p180/bin/rails server -p 3456 &");
    //    QTest::qWait(4000);
    return QUrl("http://localhost:3456/states/1");
}

void SynchronizerTest::run(QString command) const
{
    if (system(command.toAscii().data()) == 0){
        qDebug() << "success: " << command;
    }else{
        qDebug() << "failed: " << command;
    }

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
    QUrl tracker_url = startTracker();
    QUrl self_url = QUrl("http://localhost:54678/reponame");
    QString local_path = setupDirWithFiles();
    LocalDiskRepo repo(0, local_path);
    repo.initialize();
    StateDiff diff = repo.detectChanges();
    QVERIFY2(diff.numChanges() == 2, "should have 2 changes");
    QVERIFY2(repo.logger()->commitChanges(diff), "changes should be commited");

    Synchronizer sync(0, &repo, tracker_url, self_url);
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

    QVERIFY2(sync.stateDiffsToPush().count() == 0, "incorrect number of statediffs to push after loop");
    QVERIFY2(sync.stateDiffsToPull().count() == 0, "incorrect number of statediffs to pull affer loop");

    /*
      ok so now we should be able to sync a empty repo to the url and get the state diffs
      that we pushed earlier. lets try
      */

    LocalDiskRepo repo2(0, setupEmptyDir("pull_repo"));
    repo2.initialize();
    diff = repo2.detectChanges();
    QVERIFY2(diff.numChanges() == 0, "new localdiskrepo should not have changes");
    QUrl self_url2("http://localhost:838383/notreal");
    Synchronizer sync2(0, &repo2, tracker_url, self_url2);
    QVERIFY2(sync2.isReady(), "new syncer is not ready");
    sync2.requestHistory();
    waitAndListen(); // wait for tacker response.. that will trigger historyrecieved
    QVERIFY2(sync2.stateDiffsToPull().count() == 1, "should have a statediff to pull");
    sync2.loop();
    waitAndListen();
    QVERIFY2(sync2.stateDiffsToPull().count() == 0, "should have pulled statediff");


    // check that we can registerSelf and also get peerlist
    sync.registerSelf();
    sync2.registerSelf();
    waitAndListen();
    sync.requestPeers();
    waitAndListen();
    qDebug() << sync.peerUrlStrings();
    qDebug() << self_url2.toString();
    QVERIFY2(sync.peerUrlStrings().contains(self_url2.toString()), "should have other sync as peer");


    /*
      ok so now there is a statediff to pull.. and it is going to tell us to download a couple
      of files if we wanna be a cool kid.
      WHAT TO DO?
      well.. we need to get that statediff..
      then we need to push the changes into the repo

      if repo.detectChanges.size() == 0
        if repo.state().canPerformChanges(diff)
            repo.performChanges(diff)

      that would work fine.. except for newfile source resolution..
      when a repo wants to perform an addfile..
      it needs to request the file from some service
        FileFinder
            tracker_url
                get http://localhost:3456/urls_for/?file_path,size,fingerprint
                returns json array of urls to find the file

            requestFile(file_info,
      */
}

QTEST_MAIN(SynchronizerTest);

#include "tst_synchronizertest.moc"
