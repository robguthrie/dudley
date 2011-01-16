#ifndef FILEREPOSTATELOGGER_H
#define FILEREPOSTATELOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "state.h"

class StateLogger : public QObject
{
    Q_OBJECT
public:
    StateLogger(QObject* parent, QString logsDir);
    bool initialize() const;
    bool isReady() const;
    State state() const;
    QString logsDir() const;
    bool commitChanges();
    void playAllLogs(State* state);
    QByteArray openLog(QString name);
    void playLogFile(QString name, State* state);
    bool writeLogFile(QString commit_name, QString body);
    void printChanges();
    void addFile(FileInfo* fi);
    void addFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void modifyFile(FileInfo* fi);
    void modifyFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void removeFile(QString file_path);
    void renameFile(QString file_path, QString new_file_path);
    QStringList logNames() const;
    bool hasLogFile(QString name) const;

private:
    State* m_state;
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
