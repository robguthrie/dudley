#include "statelogger.h"
#include <QDir>
#include <iostream>
#include "output.h"
#include <QCryptographicHash>
#include <QDirIterator>
// logger is a file state history log
StateLogger::StateLogger(QObject* parent, QString logsDir)
    :QObject(parent), m_logsDir(logsDir)
{
    m_state = new State(this);
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

State StateLogger::state() const
{
    return m_state;
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
            g_log->debug("opening log file:"+logFilePath(name));
           return file.readAll();
        }else{
            g_log->error("could not open file:"+logFilePath(name));
        }
    }else{
        g_log->error("file not found: "+logFilePath(name));
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
    QTextStream in(openLog(name));
    while (!in.atEnd()){
        //  operation, filepath
        //  add_file, filepath, mtime(ISO), size, sha1
        //  remove_file, filepath
        //  move_file, filepath, newfilepath

        QStringList parts = splitLogLine(in.readLine());
        QString operation = parts[0];
        QString filePath = parts[1];
        if ((operation == "add_file") || (operation == "modify_file")){
            QDateTime modifiedAt = QDateTime::fromString(parts[2], Qt::ISODate);
            qint64 sizeInBytes = parts[3].toULongLong();
            QString sha1 = parts[4];
            if (operation == "add_file"){
                m_state->addFile(filePath, sizeInBytes, modifiedAt, sha1);
            }
            if (operation == "modify_file"){
                m_state->modifyFile(filePath, sizeInBytes, modifiedAt, sha1);
            }
        }
        if (operation == "remove_file") {
            m_state->removeFile(filePath);
        }
        if (operation == "rename_file"){
            QString newFilePath = parts[2];
            m_state->renameFile(filePath, newFilePath);
        }
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

QString StateLogger::joinLogLine(QStringList tokens)
{
   tokens.replaceInStrings(",", "DUDLEYCOMMA");
   return tokens.join(",");
}

QStringList StateLogger::splitLogLine(QString line)
{
    QStringList parts = line.split(',');
    parts.replaceInStrings("DUDLEYCOMMA", ",");
    return parts;
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

void StateLogger::addFile(FileInfo* fi)
{
    logAddOrModifyFile("add_file", fi);
}

void StateLogger::addFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
{
    logAddOrModifyFile("add_file", filePath, sizeInBytes, modifiedAt,  sha1);
}

void StateLogger::modifyFile(FileInfo* fi)
{
    logAddOrModifyFile("modify_file", fi);
}

void StateLogger::modifyFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
{
    logAddOrModifyFile("modify_file", filePath, sizeInBytes, modifiedAt, sha1);
}

void StateLogger::logAddOrModifyFile(QString operation, FileInfo* fi)
{
    logAddOrModifyFile(operation, fi->filePath(), fi->size(),
                        fi->lastModified(), fi->fingerPrint());
}

void StateLogger::logAddOrModifyFile(QString operation,
                                             QString filePath,
                                             qint64 sizeInBytes,
                                             QDateTime modifiedAt,
                                             QString sha1)
{
    QStringList tokens;
    tokens << operation << filePath << modifiedAt.toString(Qt::ISODate)
           << QString::number(sizeInBytes) << sha1;
    addLogLine(tokens);
}

void StateLogger::removeFile(QString file_path)
{
    QStringList tokens;
    tokens << "remove_file" << file_path;
    addLogLine(tokens);
}

void StateLogger::renameFile(QString file_path, QString new_file_path)
{
    QStringList tokens;
    tokens << "rename_file" << file_path << new_file_path;
    addLogLine(tokens);
}


void StateLogger::addLogLine(QStringList tokens)
{
    m_logLines << tokens;
}
