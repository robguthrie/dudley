#include <QtTest/QtTest>
#include "../../fileinfo.h"
class TestFileInfo : public QObject
{
    Q_OBJECT
private slots:
    void testFileInfoBasic();
};


void TestFileInfo::testFileInfoBasic(){
    FileInfo fi = newFileInfo();

    QVERIFY(fi.filePath() == filePath);
    QVERIFY(fi.fingerPrint() == sha1);
    QVERIFY(fi.lastModified() == modifiedAt);
    QVERIFY(fi.size() == sizeInBytes);

    // now update and test that all the changes happened
    modifiedAt = modifiedAt.addDays(4);
    sizeInBytes = sizeInBytes + 5;
    sha1 = "thumbprintttttttt";

    fi.update(modifiedAt, sizeInBytes, sha1);

    QVERIFY(fi.filePath() == filePath);
    QVERIFY(fi.fingerPrint() == sha1);
    QVERIFY(fi.lastModified() == modifiedAt);
    QVERIFY(fi.size() == sizeInBytes);

    QString newFilePath("renamednowhaha");
    fi.rename(newFilePath);
    QVERIFY(fi.filePath() == newFilePath);
}


FileInfo TestFileInfo::newFileInfo()
{
    QString filePath = "somefile.txt";
    QDateTime modifiedAt = QDateTime::currentDateTime();
    qint64 sizeInBytes = 45;
    QString sha1("fiiiiinnngerprint");

    FileInfo fi(filePath, modifiedAt, sizeInBytes, sha1);
    return fi;
}

//untested:
//bool FileInfo::isIdenticalTo(FileInfo *fi)
//bool FileInfo::seemsIdenticalTo(QFileInfo q){

QTEST_MAIN(TestFileInfo)
#include "testfileinfo.moc"
