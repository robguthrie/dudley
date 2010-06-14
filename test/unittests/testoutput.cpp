#include <QtTest/QtTest>
#include "../../output.h"
class TestOutput : public QObject
{
    Q_OBJECT
private slots:
    void debug();
};

void TestOutput::debug()
{
    Output::debug("something debug");
    QVERIFY(true);
}

QTEST_MAIN(TestOutput)
#include "testoutput.moc"
