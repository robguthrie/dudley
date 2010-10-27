#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../localdiskrepo.h"
#include "../../output.h"
#include "../../fileinfo.h"

Output* g_log = new Output();

class LocalDiskRepoTest : public QObject
{
    Q_OBJECT

public:
    LocalDiskRepoTest();

private Q_SLOTS:
    void testLocalDiskRepo();
    void testHttpRequest();
};

LocalDiskRepoTest::LocalDiskRepoTest()
{
}

void LocalDiskRepoTest::testLocalDiskRepo()
{
    system("rm -rf ../../testrepo/.dudley");
    LocalDiskRepo* repo = new LocalDiskRepo(0, "../../testrepo", "testrepo");

    // reset the logger
    // test it is not ready
    QVERIFY(repo->isReady() == false);

    // initialize it
    QVERIFY(repo->initialize());

    // test it is ready now
    QVERIFY(repo->isReady());

    // do an update
    repo->updateState(true);

    // test we have a file
    QVERIFY(repo->hasFileInfoByFilePath("itd-last-longer.gif"));

    // test we get a fileinfo
    FileInfo* file_info = repo->fileInfoByFilePath("itd-last-longer.gif");
    QVERIFY(file_info->filePath() == "itd-last-longer.gif");

    // get we can open the file
    QIODevice* f = repo->getFile(file_info);
    QVERIFY(f != 0);
    f->close();

}

void LocalDiskRepo::testHttpRequest()
{
    // want to use saved requests and load them into the httprequest object
    HttpMessage* message = new HttpMessage();

    QByteArray short_message =
    "dumbheader: dumbvalue\r\n"
    "content-length: 30 bytes"
    "\r\n"
    "123456789012345678901234567890";


}


QTEST_APPLESS_MAIN(LocalDiskRepoTest);

#include "tst_localdiskrepotest.moc"
