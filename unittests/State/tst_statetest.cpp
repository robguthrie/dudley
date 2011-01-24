#include <QtCore/QString>
#include <QtTest/QtTest>
#include "state.h"
class StateTest : public QObject
{
    Q_OBJECT

public:
    StateTest();

private Q_SLOTS:
    void testCase1();
};

StateTest::StateTest()
{
}

void StateTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(StateTest);

#include "tst_statetest.moc"
