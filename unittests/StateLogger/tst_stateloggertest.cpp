#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../statelogger.h"
class StateLoggerTest : public QObject
{
    Q_OBJECT

public:
    StateLoggerTest();

private Q_SLOTS:
    void testInitialize();
    void testCreateLog();
};

StateLoggerTest::StateLoggerTest()
{
}

void StateLoggerTest::testInitialize()
{
    QString logsDir = QDir::currentPath() + "/logs_dir/";
    system(QString("rm -r %1").arg(logsDir).toAscii().data());
    StateLogger* logger = new StateLogger(0, logsDir);
    QVERIFY2(logger->initialize(), "Failed to initialize logger");
}

void StateLoggerTest::testCreateLog()
{
    QString logs_dir = QDir::currentPath() + "/logs_dir/";
    StateLogger logger(0, logs_dir);
    QVERIFY2(logger.isReady(), "Logger is not ready");
    logger.addFile("prettyphoto.jpg", 512, QDateTime::currentDateTime(), "1234567890");
    logger.addFile("holidaysnap.jpg", 1024, QDateTime::currentDateTime(), "0987654321");
    bool changes_accepted = logger.acceptChanges();
    QVERIFY2(changes_accepted, "failed to accept changes.. probably could not write file");
    /* check that the state contains the info
       then drop it. then create a new logger, reading the log file
       and check again that the state contains the file
       */
    State* state = logger.state();
    QVERIFY2(state->knownFilePath("prettyphoto.jpg"), "file added was not found in state! wacky");
    QVERIFY2(state->knownFilePath("holidaysnap.jpg"), "file added was not found in state! wacky");
    //////
    //QString logs_dir = QDir::currentPath() + "/logs_dir/";
    StateLogger logger2(0, logs_dir);
    QVERIFY2(logger2.state()->knownFilePath("holidaysnap.jpg"), "file should have been in reloaded state");
    QVERIFY2(logger2.state()->knownFilePath("prettyphoto.jpg"), "file should have been in reloaded state");
    QVERIFY2(logger2.state()->knownFilePath("holidaysnap.jpg"), "file should have been in reloaded state");
    logger2.removeFile("prettyphoto.jpg");
    logger2.modifyFile("holidaysnap.jpg", 578, QDateTime::currentDateTime(), "changedprint");
    QVERIFY2(logger2.acceptChanges(), "changes were not accepted");
    bool not_here = logger2.state()->knownFilePath("prettyphoto.jpg");
    QVERIFY2(!not_here, "prettyphoto.jpg should not be present but it is");
    QString changed_print = logger2.state()->fileInfoByFilePath("holidaysnap.jpg").fingerPrint();
    qDebug() << "changed print:" << changed_print;
    bool correct_fp = logger2.state()->fileInfoByFilePath("holidaysnap.jpg").fingerPrint() == "changedprint";
    QVERIFY2(correct_fp, "the fingerprint modified file holidaysnap is incorrect");

    logger2.renameFile("holidaysnap.jpg", "crappypic.jpg");
    QVERIFY2(logger2.acceptChanges(), "changes were not accepted");
    QVERIFY2(logger2.state()->knownFilePath("crappypic.jpg"), "file rename is broken");
}

QTEST_APPLESS_MAIN(StateLoggerTest);

#include "tst_stateloggertest.moc"
