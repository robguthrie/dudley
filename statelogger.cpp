#include "statelogger.h"
#include <QDir>
#include <iostream>
#include <QCryptographicHash>
#include <QDirIterator>

#include "qobjecthelper.h"
#include "serializer.h"
#include "statediff.h"
#include "statediffop.h"
#include "parser.h"


// logger is a file state history log
StateLogger::StateLogger(QObject* parent, QString logsDir)
    :QObject(parent), m_logsDir(logsDir)
{
    m_state = new State(this);
    m_diff = new StateDiff(this);
}

bool StateLogger::initialize() const
{
    QDir dir;
    return dir.mkpath(m_logsDir);
}

bool StateLogger::isReady() const
{
    QDir dir;
    return dir.exists(m_logsDir);
}

QString StateLogger::logsDir() const
{
    return m_logsDir;
}

QStringList StateLogger::logNames() const
{
    QDir dir(m_logsDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList filenames = dir.entryList();
    QStringList lognames;
    QRegExp valid_rx("(\\d+)\\.log");
    foreach(QString name, filenames){
        if (valid_rx.exactMatch(name)){
            lognames << valid_rx.cap(1);
        }
    }
    return lognames;
}

bool StateLogger::hasLogFile(QString name) const
{
    return QFile::exists(logFilePath(name));
}

State* StateLogger::state() const
{
    return m_state;
}

StateDiff* StateLogger::diff() const
{
    return m_diff;
}

// given a state object.. play the list of file operations stored
// across each log file representing the history, (they each represent a commit)
// so that the state object represents the last
// known state of the directory we track
void StateLogger::reload()
{
    QStringList log_names = logNames();
    foreach(QString name, log_names){
        playLogFile(name);
    }
}

QByteArray StateLogger::openLog(QString name)
{
    if (hasLogFile(name)){
        QFile file(logFilePath(name));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug("opening log file:"+logFilePath(name));
           return file.readAll();
        }else{
            qDebug("could not open file:"+logFilePath(name));
        }
    }else{
        qDebug("file not found: "+logFilePath(name));
    }
    return QByteArray();
}

/* open a logfile and read it. line by line. thus startign the process of
   updating the in-memory representation of the state

   within this you will see calls to state->dosomethingFileSilent
   the silent is to prevent this state building process to be the same as
   recording new state
*/
void StateLogger::playLogFile(QString name)
{
    /*
     so we are going to open this file.. containing a statediff object..
     */
    QJson::Parser parser;
    bool ok;
    QVariant v = parser.parse(openLog(name), &ok);
    if (!ok) {
      qFatal("An error occurred during logfile parsing");
      exit (1);
    }
    StateDiff* sd;
    QJson::QObjectHelper::qvariant2qobject(v.toMap(), sd);
    playDiff(sd);
}

void StateLogger::playDiff(StateDiff* diff)
{

    StateDiffOp* sdo = 0;
    foreach(sdo, diff->diffOpPtrs()){
        sdo->preformUpon(m_state);
    }
}

// show the operations staged to be written to a history commit file.
void StateLogger::printChanges()
{
    if (m_logLines.size() > 0){
        g_log->info("Changes waiting to be written to log:"+pendingLogLines().join("\n"));
    }
}

QStringList StateLogger::pendingLogLines(){
    QStringList log_lines;
    foreach(QStringList tokens, m_logLines){
        log_lines << joinLogLine(tokens);
    }
    return log_lines;
}


bool StateLogger::commitChanges(){
    if (m_logLines.size() > 0){
        QString commit_name = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
        QString body = pendingLogLines().join("\n");
        if (writeLogFile(commit_name, body)){
            g_log->debug("writeLogFile returned true");
            m_logLines.clear();
            return true;
        }
    }
    return false;
}

// flush the staged operations to a commit log file
// should probably do the operation, fileInfo* to log line here..
bool StateLogger::writeLogFile(QString name, QString body)
{
    if (!QFile::exists(m_logsDir)){
        g_log->error("could not open log dir");
        return false;
    }

    // refuse to save a log with a funny name
    QRegExp name_rx("\\d+");
    if (!name_rx.exactMatch(name)){
        g_log->error("log filename is not just digits: "+name);
        return false;
    }

    QFile file(logFilePath(name));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Append)){
        QTextStream out(&file);
        out << body;
        g_log->debug(QString::number(body.size()).append(" bytes written to logfile"));
        file.close();
        return true;
    }else{
        g_log->error("could not write log: "+logFilePath(name));
        return false;
    }
}

QString StateLogger::logFilePath(QString name)
{
    return QString(m_logsDir+"/"+name+".log");
}

void StateLogger::addLogLine(QStringList tokens)
{
    m_logLines << tokens;
}
