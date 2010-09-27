#ifndef FILEREPOSTATE_H
#define FILEREPOSTATE_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QDateTime>
#include "fileinfo.h"

class FileRepoStateLogger;

class FileRepoState : public QObject
{
    Q_OBJECT
public:
    FileRepoState(QObject* parent = 0);
    FileRepoState(QObject* parent, QString logs_dir);
    ~FileRepoState();
    void reload();
    FileRepoStateLogger* logger();
    void setLogger(FileRepoStateLogger* logger);
    void importLog(QString name, QString body);
    bool commitChanges();
    bool containsFileInfo(FileInfo file_info);
    bool containsFilePath(QString file_path);
    bool containsFingerPrint(QString finger_print);
    FileInfo* fileInfoByFilePath(QString file_path);
    FileInfo* fileInfoByFingerPrint(QString sha1);

    // manipulation
    void addFile(FileInfo* file_info);
    void addFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void modifyFile(FileInfo* file_info);
    void modifyFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    bool removeFile(QString filePath);
    bool renameFile(QString filePath, QString newFilePath);

    // query
    QList<FileInfo*> filesInDir(QString path);
    QStringList subDirs(QString path);
//    QHash<QString, FileInfo*> findFiles(QString query = "");
    QHash<QString, FileInfo*> knownFiles(QStringList found_files);
    QHash<QString, FileInfo*> missingFiles(QStringList found_files);
    QStringList missingFilePaths(QStringList found_files);
    QStringList knownFilePaths(QStringList found_files);
    QStringList unknownFilePaths(QStringList found_files);

private:
    FileRepoStateLogger* m_logger;
    QHash<QString, FileInfo*> m_files;
    QHash<QString, FileInfo*> m_fingerprints;
    bool m_logChanges;
};

#endif // FILEREPOSTATE_H
