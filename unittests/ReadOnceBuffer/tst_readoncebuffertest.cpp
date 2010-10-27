#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../readoncebuffer.h"
#include "../../output.h"

Output* g_log = new Output();

class ReadOnceBufferTest : public QObject
{
    Q_OBJECT

public:
    ReadOnceBufferTest();

private Q_SLOTS:
    void testCase1();
};

ReadOnceBufferTest::ReadOnceBufferTest()
{
}

void ReadOnceBufferTest::testCase1()
{
    ReadOnceBuffer* rob = new ReadOnceBuffer();
    QByteArray ba = "hello this is a mega byte";
    qint64 bw = 0;
    rob->open(QIODevice::ReadWrite | QIODevice::Append);
    bw = rob->write(ba);
    qDebug() << "bytes written: " << bw;
    QVERIFY(rob->size() == ba.size());
    ba = rob->readAll();
    qDebug() << ba;
    QVERIFY(ba == "hello this is a mega byte");
}

QTEST_APPLESS_MAIN(ReadOnceBufferTest);

#include "tst_readoncebuffertest.moc"
