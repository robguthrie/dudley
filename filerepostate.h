#ifndef FILEREPOSTATE_H
#define FILEREPOSTATE_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QDateTime>
#include "fileinfo.h"

class FileRepoStateLogger;

class FileRepoState
{
public:
    FileRepoState(FileRepoStateLogger* logger);
    FileRepoStateLogger* logger();
    void reload();
    bool containsFilePath(QString file_path);
    FileInfo* fileInfoByFilePath(QString file_path);
    FileInfo* fileInfoByFingerPrint(QString sha1);

    void addFile(FileInfo* file_info);
    void addFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void addFileSilent(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void modifyFile(FileInfo* file_info);
    void modifyFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void modifyFileSilent(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    bool removeFile(QString filePath);
    bool removeFileSilent(QString filePath);
    bool renameFile(QString filePath, QString newFilePath);
    bool renameFileSilent(QString filePath, QString newFilePath);

    QHash<QString, FileInfo*> knownFiles(QStringList found_files);
    QHash<QString, FileInfo*> missingFiles(QStringList found_files);
    QStringList missingFilePaths(QStringList found_files);
    QStringList knownFilePaths(QStringList found_files);
    QStringList unknownFilePaths(QStringList found_files);

private:
    FileRepoStateLogger* m_logger;
    QHash<QString, FileInfo*> m_files;
    QHash<QString, FileInfo*> m_fingerprints;
};

#endif // FILEREPOSTATE_H
