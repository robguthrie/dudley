#ifndef FILEREPOSTATE_H
#define FILEREPOSTATE_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QDateTime>
#include "fileinfo.h"
#include "statediff.h"
#include <QDebug>

class State
{
public:
    QList<FileInfo> filesInDir(QString path) const;
    QStringList subDirectories(QString path) const;
    bool knownFilePath(QString path) const;
    bool knownFingerPrint(QString finger_print) const;
    QStringList knownFilePaths(QStringList file_paths) const;
    QStringList unknownFilePaths(QStringList file_paths) const;
    QStringList missingFilePaths(QStringList file_paths) const;
    FileInfo fileInfoByFilePath(QString file_path) const;
    FileInfo fileInfoByFingerPrint(QString sha1) const;
    bool canPerformChanges(StateDiff &sd) const;
    void performChanges(StateDiff &sd);
    void clear();

protected:
    bool canAddFile(QString file_path) const;
    bool canModifyFile(QString file_path) const;
    bool canRenameFile(QString file_path, QString new_file_path) const;
    bool canRemoveFile(QString file_path) const;

    void addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void removeFile(QString file_path);
    void renameFile(QString file_path, QString new_file_path);

    QHash<QString, FileInfo> m_files;
    QHash<QString, FileInfo> m_fingerprints;
};

#endif // FILEREPOSTATE_H
