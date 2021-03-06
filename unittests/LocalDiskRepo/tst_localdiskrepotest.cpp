#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../localdiskrepo.h"
class LocalDiskRepoTest : public QObject
{
    Q_OBJECT

public:
    LocalDiskRepoTest();

private Q_SLOTS:
    void testCreateLocalDiskRepo();
};

LocalDiskRepoTest::LocalDiskRepoTest()
{
}

void LocalDiskRepoTest::testCreateLocalDiskRepo()
{
    QStringList commands;
    QString repo_dir = QDir::currentPath()+"/test_repo";
    QString file1_path = repo_dir+"/file1.txt";
    QString file2_path = repo_dir+"/file2.txt";
    QString file3_path = repo_dir+"/file2renamed.txt";

    commands << QString("rm -r %1").arg(repo_dir);
    commands << QString("mkdir %1").arg(repo_dir);
    commands << QString("echo \"something like the contents of a file\" > %1").arg(file1_path);
    commands << QString("echo \"another file with a wonderful message\" > %1").arg(file2_path);
    QString command;
    foreach(command, commands){
        system(command.toAscii().data());
    }
    commands.clear();

    LocalDiskRepo ldr(0, repo_dir);
    QVERIFY2(ldr.type()== "LocalDisk", "Wrong type of repo");
    QVERIFY2(ldr.initialize(), "could not initialse test repo");
    QVERIFY2(ldr.isReady(), "Repo is not ready");
    StateDiff diff = ldr.detectChanges();

    QVERIFY2(diff.numChanges() == 2, "incorrect number of changes detected (add, add expected)");
    QVERIFY2(ldr.logger()->commitChanges(diff), "could not accept changes");
    QVERIFY2(ldr.canReadData(), "Cant read the repo dir");

    QFileInfo file1_qfi(file1_path);
    FileInfo file1_fi("file1.txt", file1_qfi.size(),
                      file1_qfi.lastModified(), "71db4e0b7279201aabfb012fdfad9e1e5d3d6422");
    QFileInfo file2_qfi = QFileInfo(file2_path);
    FileInfo file2_fi("file2.txt", file2_qfi.size(),
                     file2_qfi.lastModified(), "d408accd4df738659bb0ff01189b905ade576250");

    QVERIFY2(ldr.fileExists(file1_fi), "File1 cannot be found");
    QVERIFY2(ldr.fileExists(file2_fi), "File2 cannot be found");

    commands << QString("echo \"oh baby you've changed\" > %1").arg(file1_path);
    commands << QString("mv %1 %3").arg(file2_path, file3_path);

    foreach(command, commands){
        system(command.toAscii().data());
    }
    commands.clear();
    diff = ldr.detectChanges();
    QVERIFY2(diff.numChanges() == 2,"incorrect number of changes (modfiy, rename expected)");
    QVERIFY2(ldr.logger()->commitChanges(diff), "could not accept changes");

    command = QString("rm %1").arg(file1_path);
    system(command.toAscii().data());
    diff = ldr.detectChanges();
    QVERIFY2(diff.numChanges() == 1, "delete not detected");
    QVERIFY2(ldr.logger()->commitChanges(diff), "changes not accepted");
}

QTEST_APPLESS_MAIN(LocalDiskRepoTest);

#include "tst_localdiskrepotest.moc"
