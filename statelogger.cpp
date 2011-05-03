#include "statelogger.h"
#include <QDir>
#include <iostream>
#include <QCryptographicHash>
#include <QDirIterator>
#include "stateop.h"


// logger is a file state history log
StateLogger::StateLogger(QObject* parent, QString logsDir)
    :QObject(parent), m_logsDir(logsDir)
{
    m_state = new State(this);
    m_stateDiff = new StateDiff();
    reload();
}

StateLogger::~StateLogger()
{
//    delete m_state;
//    delete m_stateDiff;
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
StateDiff* StateLogger::stateDiff() const
{
    return m_stateDiff;
}
/* call this to log that a file has been added to a repo */
void StateLogger::addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    StateOp op;
    op.setAction("AddFile");
    op.setFilePath(file_path);
    op.setSize(size);
    op.setModifiedAt(modified_at);
    op.setFingerPrint(sha1);
    m_stateDiff->stateOps()->append(op);
}

/* call this to log that a file has been modified in a repo */
/* TODO: check that the file exists in the state.. throw a critical or fatal if it does not */
void StateLogger::modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    StateOp op;
    op.setAction("ModifyFile");
    op.setFilePath(file_path);
    op.setSize(size);
    op.setModifiedAt(modified_at);
    op.setFingerPrint(sha1);
    m_stateDiff->stateOps()->append(op);
}

void StateLogger::removeFile(QString file_path)
{
    StateOp op;
    op.setAction("RemoveFile");
    op.setFilePath(file_path);
    m_stateDiff->stateOps()->append(op);
}

void StateLogger::renameFile(QString file_path, QString new_file_path)
{
    StateOp op;
    op.setAction("RenameFile");
    op.setFilePath(file_path);
    op.setNewFilePath(new_file_path);
    m_stateDiff->stateOps()->append(op);
}

/* merge any changes into the state, and save a new diff log to disk */
bool StateLogger::acceptChanges(){
    int num_changes = m_stateDiff->numChanges();
    if (num_changes > 0){
        if (saveStateDiff(m_stateDiff)){
            preformChangesOnState(m_stateDiff);
            delete m_stateDiff;
            m_stateDiff = new StateDiff();
        }
    }
    return num_changes;
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
            preformChangesOnState(&sd);
        }
    }
}

void StateLogger::preformChangesOnState(StateDiff *sd)
{
    QList<StateOp> *state_ops = sd->stateOps();
    StateOp op;
    foreach(op, *state_ops){
        QString action = op.action();
        if (action == "AddFile"){
            m_state->addFile(op.filePath(), op.size(), op.modifiedAt(), op.fingerPrint());
        }else if (action == "ModifyFile"){
            m_state->modifyFile(op.filePath(), op.size(), op.modifiedAt(), op.fingerPrint());
        }else if (action == "RenameFile"){
            m_state->renameFile(op.filePath(), op.newFilePath());
        }else if (action == "RemoveFile"){
            m_state->removeFile(op.filePath());
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

bool StateLogger::saveStateDiff(StateDiff *state_diff) const
{
    QFile file(tmpLogFilePath());
    if (file.open(QIODevice::WriteOnly)){
        file.write(state_diff->toJSON());
        file.rename(logFilePath(state_diff->name()));
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
