#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../upnpwrapper.h"
class UpnpWrapperTest : public QObject
{
    Q_OBJECT

public:
    UpnpWrapperTest();

private Q_SLOTS:
    void testCase1();
};

UpnpWrapperTest::UpnpWrapperTest()
{
}

void UpnpWrapperTest::testCase1()
{
    UpnpWrapper uw(this);
    uw.addRedirect();
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(UpnpWrapperTest);

#include "tst_upnpwrappertest.moc"
