#ifndef FILEREPOSTATELOGGER_H
#define FILEREPOSTATELOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "state.h"

class StateDiff;
class StateLogger : public QObject
{
    Q_OBJECT
public:
    StateLogger(QObject* parent, QString logsDir);
    bool initialize() const;
    bool isReady() const;
    State* state() const;
    StateDiff* diff() const;
    QString logsDir() const;
    bool commitChanges();
    void reload();
    QByteArray openLog(QString name);
    void playLogFile(QString name, State* state);
    bool writeLogFile(QString commit_name, QString body);
    void printChanges();

    QStringList logNames() const;
    bool hasLogFile(QString name) const;

private:
    State* m_state;
    StateDiff* m_diff;
    QString m_logsDir;

    void addOrModifyFile(QString action, FileInfo* fi);
    void addOrModifyFile(QString action, QString filePath, qint64 size, QDateTime modifiedAt, QString sha1);
    QString logFilePath(QString name);
};



#endif // FILEREPOSTATELOGGER_H
