#ifndef FILEREPOSTATELOGGER_H
#define FILEREPOSTATELOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "repostate.h"

class RepoStateLogger
{

public:
    RepoStateLogger(QString logsDir);
    bool initialize();
    bool isReady() const;
    QString logsDir();
//    FileRepoState loadState();
    void loadState(RepoState* state);
    QString logsDir() const;
    bool commitChanges();
    void playAllLogs(RepoState* state);
    QByteArray openLog(QString name);
    void playLogFile(QString name, RepoState* state);
    bool writeLogFile(QString commit_name, QString body);
    void printChanges();
    void logAddFile(FileInfo* fi);
    void logAddFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void logModifyFile(FileInfo* fi);
    void logModifyFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void logRemoveFile(QString file_path);
    void logRenameFile(QString file_path, QString new_file_path);
    QStringList logNames();
    bool hasLogFile(QString name);

private:
    QStringList pendingLogLines();
    QString joinLogLine(QStringList tokens);
    QStringList splitLogLine(QString line);

    QString m_logsDir;
    QList<QStringList> m_logLines;

    void logAddOrModifyFile(QString operation, FileInfo* fi);
    void logAddOrModifyFile(QString operation, QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void addLogLine(QStringList tokens);
    QString logFilePath(QString name);
};



#endif // FILEREPOSTATELOGGER_H
