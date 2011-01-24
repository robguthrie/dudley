#include <QtCore/QString>
#include <QtTest/QtTest>
#include "qobjecthelper.h"
#include "serializer.h"
#include "statediff.h"
#include "statediffop.h"
#include "parser.h"

class StateDiffTest : public QObject
{
    Q_OBJECT

public:
    StateDiffTest();

private Q_SLOTS:
    void testCase1();
};

StateDiffTest::StateDiffTest()
{
}

void StateDiffTest::testCase1()
{
    StateDiff stateDiff1;
    stateDiff1.setName("testDiff");
    qDebug() << "here i am 1";
    StateDiffOp sdo(&stateDiff1);
    sdo.setAction(StateDiffOp::AddFile);
    sdo.setFilePath("test.txt");
    sdo.setFingerPrint("ribbed");
    sdo.setLastModified(QDateTime::currentDateTime());
    qDebug() << "here i am2";
    stateDiff1.appendOp(&sdo);

    // now try to serialize the statediff
    QVariantMap stateDiff1_vmap = QJson::QObjectHelper::qobject2qvariant(&stateDiff1);
    QJson::Serializer serializer;
    QByteArray stateDiff1_json(serializer.serialize(stateDiff1_vmap));
    qDebug() << "here i am3";
    // now back the other way...
    QJson::Parser parser;
    bool ok;
    qDebug() << "here i am4";
    QVariantMap stateDiff2_vmap = parser.parse(stateDiff1_json, &ok).toMap();
    if (!ok) {
      qFatal("An error occurred during parsing");
      exit (1);
    }
    qDebug() << "here i am5";
    StateDiff stateDiff2;
    QJson::QObjectHelper::qvariant2qobject(stateDiff2_vmap, &stateDiff2);
    qDebug() << "here i am6";
    QVERIFY2(stateDiff2.name() == stateDiff1.name(), "Serialization Failure");
    qDebug() << "here i am7";
}

QTEST_APPLESS_MAIN(StateDiffTest);

#include "tst_statedifftest.moc"
