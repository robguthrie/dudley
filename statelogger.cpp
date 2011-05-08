#include "statelogger.h"
#include <QDir>
#include <iostream>
#include <QCryptographicHash>
#include <QDirIterator>
#include "stateop.h"


// logger is a file state history log
StateLogger::StateLogger(QString logsDir)
    :m_logsDir(logsDir)
{
    m_state = new State();
    reload();
}

StateLogger::~StateLogger()
{
    delete m_state;
}

/* create directory to store log files
 * only needs to be done for first time a logger is used with a repo */
bool StateLogger::initialize() const
{
    QDir dir;
    return dir.mkpath(m_logsDir);
}

/* check if logs directory is there, and anything else that needs to be ready later */
bool StateLogger::isReady() const
{
    QDir dir;
    return dir.exists(m_logsDir);
}

/* return the path to the logs dir */
QString StateLogger::logsDir() const
{
    return m_logsDir;
}

/* return the state object */
State* StateLogger::state() const
{
    return m_state;
}

/* merge any changes into the state, and save a new diff log to disk */
bool StateLogger::commitChanges(StateDiff &state_diff)
{
    if (m_state->canPerformChanges(state_diff)){
        if (saveStateDiff(state_diff)){
            m_state->performChanges(state_diff);
            return true;
        }
    }
    return false;
}

/* reload the state from disk, and drop any changes */
void StateLogger::reload()
{
    m_state->clear();
    QStringList log_names = logNames();
    foreach(QString name, log_names){
        bool ok;
        StateDiff sd = loadStateDiff(name, &ok);
        if(ok){
            m_state->performChanges(sd);
        }
    }
}

StateDiff StateLogger::loadStateDiff(QString name, bool *ok) const
{
    StateDiff sd;
    *ok = false;
    if (hasLogFile(name)){
        QFile file(logFilePath(name));
        if (file.open(QIODevice::ReadOnly)) {
            (*ok) = sd.fromJSON(file.readAll());
        }
    }
    qWarning() << "could not open file:" << logFilePath(name);
    return sd;
}

bool StateLogger::saveStateDiff(StateDiff &state_diff) const
{
    QFile file(tmpLogFilePath());
    if (file.open(QIODevice::WriteOnly)){
        file.write(state_diff.toJSON());
        file.rename(logFilePath(state_diff.name()));
        file.close();
        return true;
    }
    qCritical() << "could not write log: ";
    return false;
}

bool StateLogger::hasLogFile(QString name) const
{
    return QFile::exists(logFilePath(name));
}

QString StateLogger::logFilePath(QString name) const
{
    return QString(m_logsDir+"/"+name+".log");
}

QString StateLogger::tmpLogFilePath() const
{
    return QString(m_logsDir+"/to_be_renamed.log");
}

QStringList StateLogger::logNames() const
{
    QDir dir(m_logsDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList filenames = dir.entryList();
    QStringList lognames;
    QRegExp valid_rx("(\\S+)\\.log");
    foreach(QString name, filenames){
        if (valid_rx.exactMatch(name)){
            lognames << valid_rx.cap(1);
        }else{
            qDebug() << " invalid name log file name" << name;
        }
    }
    return lognames;
}
