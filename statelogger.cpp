#include "statelogger.h"
#include <QDir>
#include <iostream>
#include <QCryptographicHash>
#include <QDirIterator>
#include "qobjecthelper.h"
#include "serializer.h"
#include "stateop.h"
#include "parser.h"


// logger is a file state history log
StateLogger::StateLogger(QObject* parent, QString logsDir)
    :QObject(parent), m_logsDir(logsDir)
{
    m_state = new State(this);
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

/* call this to log that a file has been added to a repo */
void StateLogger::addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    StateOp op;
    op.setAction("AddFile");
    op.setFilePath(file_path);
    op.setSize(size);
    op.setModifiedAt(modified_at);
    op.setFingerPrint(sha1);
    m_stateOps << op;
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
    m_stateOps << op;
}

void StateLogger::removeFile(QString file_path)
{
    StateOp op;
    op.setAction("RemoveFile");
    op.setFilePath(file_path);
    m_stateOps << op;
}

void StateLogger::renameFile(QString file_path, QString new_file_path)
{
    StateOp op;
    op.setAction("RenameFile");
    op.setFilePath(file_path);
    op.setNewFilePath(new_file_path);
    m_stateOps << op;
}

/* merge any changes into the state, and save a new diff log to disk */
int StateLogger::acceptChanges(){
    int num_changes = m_stateOps.size();
    if (num_changes > 0){
        QByteArray body = serializeStateOps(m_stateOps);
        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(body);
        QString commit_name = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
        commit_name.append(hash.result().toHex());
        if (writeLogFile(commit_name, body)){
            preformChangesOnState(m_stateOps);
            m_stateOps.clear();
            qDebug() << "wrote log file: " << commit_name;
        }else{
            qCritical() << "could not write log file: " << commit_name;
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
        QByteArray json_ba = readLogFile(name);
        QList<StateOp> state_ops = deserializeStateOps(json_ba);
        preformChangesOnState(state_ops);
    }
}

void StateLogger::preformChangesOnState(QList<StateOp> &state_ops)
{
    StateOp op;
    foreach(op, state_ops){
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

QByteArray StateLogger::serializeStateOps(QList<StateOp> &state_ops) const
{
    QVariantList list;
    StateOp op;
    foreach(op, state_ops){
        qDebug() << op.toString();
        list << op.toVariant();
    }
    QVariant v(list);
    QJson::Serializer serializer;
    return serializer.serialize(v);
}

QList<StateOp> StateLogger::deserializeStateOps(QByteArray json_ba) const
{
    QJson::Parser parser;
    bool ok;
    QVariant vlist = parser.parse(json_ba, &ok).toList();
    if (!ok) {
        qFatal("An error occurred during logfile parsing");
        exit(1);
    }
    QVariantList list = vlist.toList();
    QList<StateOp> state_ops;
    QVariant v;
    foreach(v, list){
        StateOp op;
        op.fromVariant(v);
        state_ops << op;
    }
    return state_ops;
}

QByteArray StateLogger::readLogFile(QString name) const
{
    if (hasLogFile(name)){
        QFile file(logFilePath(name));
        if (file.open(QIODevice::ReadOnly)) {
           return file.readAll();
        }
    }
    qWarning() << "could not open file:" << logFilePath(name);
    return QByteArray();
}

bool StateLogger::writeLogFile(QString name, QByteArray body) const
{
    QFile file(logFilePath(name));
    if (file.open(QIODevice::WriteOnly)){
        file.write(body);
        file.close();
        qDebug() << body.size() << " bytes written to logfile";
        return true;
    }
    qCritical() << "could not write log: " << logFilePath(name);
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

QStringList StateLogger::logNames() const
{
    QDir dir(m_logsDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList filenames = dir.entryList();
    QStringList lognames;
    QRegExp valid_rx("(\\w+)\\.log");
    foreach(QString name, filenames){
        if (valid_rx.exactMatch(name)){
            lognames << valid_rx.cap(1);
        }
    }
    return lognames;
}
