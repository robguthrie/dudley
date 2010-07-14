#include <QtTest/QtTest>
#include "../../filerepomanager.h"
class TestFileRepoManager : public QObject
{
    Q_OBJECT
private slots:
    void testCreateSettings();
    void testAddFileRepo();
    void testRemoveFileRepo();
    void testListFileRepos();
    void testFindFileWithinRepos();
};


void TestFileRepoManager::testCreateSettings(){
    FileRepoManager frm();
    frm.creat
}

QTEST_MAIN(TestFileRepoManager)
#include "testfilerepomanger.moc"

