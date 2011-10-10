#ifndef STATELOGGER_H
#define STATELOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "state.h"
#include "stateop.h"
#include "statediff.h"
class StateLogger
{
public:
    StateLogger(QString logsDir);
    ~StateLogger();
    bool initialize() const;
    bool isReady() const;
    QString logsDir() const;
    State* state() const;
    bool commitChanges(StateDiff &state_diff);
    void reload();
    QStringList logNames() const;
    bool hasStateDiff(QString name) const;
    QByteArray loadStateDiffJson(QString name) const;
    StateDiff loadStateDiff(QString name, bool *ok) const;

private:
    void playLogFile(QString name);
    void preformChangesOnState(StateDiff &sd);
    bool saveStateDiff(StateDiff &state_diff) const;
    QString logFilePath(QString name) const;
    QString tmpLogFilePath() const;

    QString m_logsDir;
    State *m_state;
};


// logger is a file state history log
#endif // STATELOGGER_H
