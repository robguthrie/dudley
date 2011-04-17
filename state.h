#ifndef FILEREPOSTATE_H
#define FILEREPOSTATE_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QDateTime>
#include "fileinfo.h"
#include <QDebug>
class StateLogger;

class State : public QObject
{
    Q_OBJECT
public:
    State(QObject* parent = 0);
    ~State();
    QStringList filesInDir(QString path);
    QStringList subDirectories(QString path);
    bool knownFilePath(QString path);
    bool knownFingerPrint(QString finger_print);
    QStringList knownFilePaths(QStringList file_paths);
    QStringList unknownFilePaths(QStringList file_paths);
    QStringList missingFilePaths(QStringList file_paths);
    FileInfo fileInfoByFilePath(QString file_path);
    FileInfo fileInfoByFingerPrint(QString sha1);
    friend class StateLogger;

protected:
    // manipulation
    void clear();
    void addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    bool removeFile(QString file_path);
    bool renameFile(QString file_path, QString new_file_path);

    QHash<QString, FileInfo> m_files;
    QHash<QString, FileInfo> m_fingerprints;
};

#endif // FILEREPOSTATE_H
