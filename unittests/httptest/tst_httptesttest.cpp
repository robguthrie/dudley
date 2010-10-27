#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

class HttptestTest : public QObject
{
    Q_OBJECT

public:
    HttptestTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

HttptestTest::HttptestTest()
{
}

void HttptestTest::initTestCase()
{
}

void HttptestTest::cleanupTestCase()
{
}

void HttptestTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(HttptestTest);

#include "tst_httptesttest.moc"
