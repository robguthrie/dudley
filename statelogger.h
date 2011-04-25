#ifndef STATELOGGER_H
#define STATELOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "state.h"
#include "stateop.h"
#include "statediff.h"
class StateLogger : public QObject
{
    Q_OBJECT
public:
    StateLogger(QObject* parent, QString logsDir);
    ~StateLogger();
    bool initialize() const;
    bool isReady() const;
    QString logsDir() const;
    State* state() const;
    StateDiff* stateDiff() const;
    void addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void removeFile(QString file_path);
    void renameFile(QString file_path, QString new_file_path);
    bool acceptChanges();
    void reload();

private:
    void playLogFile(QString name);
    void preformChangesOnState(QList<StateOp> *state_ops);
    QByteArray readLogFile(QString name) const;
    bool writeLogFile(StateDiff* state_diff) const;
    bool hasLogFile(QString name) const;
    QString logFilePath(QString name) const;
    QString tmpLogFilePath(QString name) const;
    QStringList logNames() const;

    QString m_logsDir;
    State *m_state;
    StateDiff *m_stateDiff;
};


// logger is a file state history log
#endif // STATELOGGER_H
