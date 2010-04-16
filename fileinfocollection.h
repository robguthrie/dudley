#ifndef FILEINFOCOLLECTION_H
#define FILEINFOCOLLECTION_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QDateTime>
#include "fileinfo.h"

class Logger;
class FileInfoCollection
{

private:
    QHash<QString, FileInfo*> m_files;
    QHash<QString, FileInfo*> m_fingerprints; // current fingerprints
    QString m_path;
    QList<QString> m_missingFilePaths;

    void readLogLine(QString line);
    void recursiveFind(QString path, Logger *logger, QList<FileInfo*> *unknown_files, QStringList *found_files, bool read_sha1s);
public:
    FileInfoCollection();
    FileInfoCollection(QString path);
    bool exists();
    QString path();
    FileInfo* getFileInfo(QString file_path);
    void update(QString filePath, QFileInfo *qfi, bool read_sha1);
    void update(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    bool remove(QString filePath);
    bool rename(QString filePath, QString newFilePath);
    void rename(FileInfo *fileInfo, QString newFilePath);
    bool containsFilePath(QString file_path);
    void scanForChanges(QString path, Logger *logger, bool read_sha1s, bool write_log = false, bool print_log = false);
    QString relativeFilePath(QString path);

    QHash<QString, FileInfo*> knownFiles(QStringList *found_files);
    QHash<QString, FileInfo*> missingFiles(QStringList *found_files);

    QStringList knownFilePaths(QStringList *found_files);
    QStringList unknownFilePaths(QStringList *found_files);
    QStringList missingFilePaths(QStringList *found_files);
};

#endif // FILEINFOCOLLECTION_H
