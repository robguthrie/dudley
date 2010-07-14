#ifndef FILEREPOSTATELOGGER_H
#define FILEREPOSTATELOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "filerepostate.h"

class FileRepoStateLogger
{
public:
    FileRepoStateLogger(QString logsDir);
    bool initialize();
    bool isReady() const;
    QString logsDir();
    FileRepoState loadState();
    void loadState(FileRepoState* state);
    QString logsDir() const;
    void writeLogFile();
    void printLogFile();
    void logAddFile(FileInfo* fi);
    void logAddFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void logModifyFile(FileInfo* fi);
    void logModifyFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void logRemoveFile(QString file_path);
    void logRenameFile(QString file_path, QString new_file_path);

private:

    QString m_logsDir;
    QStringList m_logLines;

    void readLogFile(QString logFilePath, FileRepoState* state);
    void logAddOrModifyFile(QString operation, FileInfo* fi);
    void logAddOrModifyFile(QString operation, QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void addLogLine(QStringList tokens);

};



#endif // FILEREPOSTATELOGGER_H
