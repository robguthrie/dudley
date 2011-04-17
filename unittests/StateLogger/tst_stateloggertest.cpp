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
    QString logsDir = QDir::currentPath() + "/logs_dir/";
    StateLogger logger(0, logsDir);
    QVERIFY2(logger.isReady(), "Logger is not ready");
    logger.addFile("prettyphoto.jpg", 512, QDateTime::currentDateTime(), "1234567890");
    logger.addFile("holidaysnap.jpg", 1024, QDateTime::currentDateTime(), "0987654321");
    bool changes_accepted = logger.acceptChanges();
    QVERIFY2(changes_accepted, "failed to accept changes.. probably could not write file");
}

QTEST_APPLESS_MAIN(StateLoggerTest);

#include "tst_stateloggertest.moc"
