#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../statelogger.h"
class StateLoggerTest : public QObject
{
    Q_OBJECT

public:
    StateLoggerTest();

private Q_SLOTS:
    void testCase1();
};

StateLoggerTest::StateLoggerTest()
{
}

void StateLoggerTest::testCase1()
{
    QString logsDir = QDir::currentPath() + "/logs_dir/";
    system(QString("rm -r %1").arg(logsDir).toAscii().data());

    StateLogger* logger = new StateLogger(0, logsDir);


    QVERIFY2(logger->initialize(), "Failed to initialize logger");
}

QTEST_APPLESS_MAIN(StateLoggerTest);

#include "tst_stateloggertest.moc"
