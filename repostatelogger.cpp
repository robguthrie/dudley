#include "repostatelogger.h"
#include <QDir>
#include <iostream>
#include "output.h"
#include <QCryptographicHash>

// logger is a file state history log
RepoStateLogger::RepoStateLogger(QString logsDir)
    :m_logsDir(logsDir) {}

bool RepoStateLogger::initialize()
{
    QDir dir;
    Output::debug(QString("creating log dir: %1").arg(m_logsDir));
    return dir.mkpath(m_logsDir);
}

bool RepoStateLogger::isReady() const
{
    QDir dir;
    return dir.exists(m_logsDir);
}

QString RepoStateLogger::logsDir() const
{
    return m_logsDir;
}

QStringList RepoStateLogger::logNames()
{
    QDir dir(m_logsDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList filenames = dir.entryList();
    QStringList lognames;
    QRegExp valid_rx("(\\d+)\\.log");
    foreach(QString name, filenames){
        if (valid_rx.exactMatch(name)) lognames << valid_rx.cap(1);
    }
    return lognames;
}

bool RepoStateLogger::hasLogFile(QString name)
{
    return QFile::exists(logFilePath(name));
}

// given a state object.. play the list of file operations stored
// across each log file representing the history, (they each represent a commit)
// so that the state object represents the last
// known state of the directory we track
void RepoStateLogger::playAllLogs(RepoState* state)
{
    QStringList log_names = logNames();
    foreach(QString name, log_names){
        playLogFile(name, state);
    }
}

QByteArray RepoStateLogger::openLog(QString name)
{
    if (hasLogFile(name)){
        QFile file(logFilePath(name));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Output::debug("opening log file:"+logFilePath(name));
           return file.readAll();
        }else{
            Output::error("could not open file:"+logFilePath(name));
        }
    }else{
        Output::error("file not found: "+logFilePath(name));
    }
    return QByteArray();
}

/* open a logfile and read it. line by line. thus startign the process of
   updating the in-memory representation of the state

   within this you will see calls to state->dosomethingFileSilent
   the silent is to prevent this state building process to be the same as
   recording new state
*/
void RepoStateLogger::playLogFile(QString name, RepoState* state)
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
                state->addFile(filePath, modifiedAt, sizeInBytes, sha1);
            }
            if (operation == "modify_file"){
                state->modifyFile(filePath, modifiedAt, sizeInBytes, sha1);
            }
        }
        if (operation == "remove_file") {
            state->removeFile(filePath);
        }
        if (operation == "rename_file"){
            QString newFilePath = parts[2];
            state->renameFile(filePath, newFilePath);
        }
    }
}



// show the operations staged to be written to a history commit file.
void RepoStateLogger::printChanges()
{
    if (m_logLines.size() > 0){
        Output::info("Changes waiting to be written to log:"+pendingLogLines().join("\n"));
    }
}

QStringList RepoStateLogger::pendingLogLines(){
    QStringList log_lines;
    foreach(QStringList tokens, m_logLines){
        log_lines << joinLogLine(tokens);
    }
    return log_lines;
}

QString RepoStateLogger::joinLogLine(QStringList tokens)
{
   tokens.replaceInStrings(",", "DUDLEYCOMMA");
   return tokens.join(",");
}

QStringList RepoStateLogger::splitLogLine(QString line)
{
    QStringList parts = line.split(',');
    parts.replaceInStrings("DUDLEYCOMMA", ",");
    return parts;
}

bool RepoStateLogger::commitChanges(){
    if (m_logLines.size() > 0){
        QString commit_name = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
        QString body = pendingLogLines().join("\n");
        if (writeLogFile(commit_name, body)){
            Output::debug("writeLogFile returned true");
            m_logLines.clear();
            return true;
        }
    }
    return false;
}

// flush the staged operations to a commit log file
// should probably do the operation, fileInfo* to log line here..
bool RepoStateLogger::writeLogFile(QString name, QString body)
{
    if (!QFile::exists(m_logsDir)){
        Output::error("could not open log dir");
        return false;
    }

    // refuse to save a log with a funny name
    QRegExp name_rx("\\d+");
    if (!name_rx.exactMatch(name)){
        Output::error("log filename is not just digits: "+name);
        return false;
    }

    QFile file(logFilePath(name));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Append)){
        QTextStream out(&file);
        out << body;
        Output::debug(QString::number(body.size()).append(" bytes written to logfile"));
        file.close();
        return true;
    }else{
        Output::error("could not write log: "+logFilePath(name));
        return false;
    }
}

QString RepoStateLogger::logFilePath(QString name)
{
    return QString(m_logsDir+"/"+name+".log");
}

void RepoStateLogger::logAddFile(FileInfo* fi)
{
    logAddOrModifyFile("add_file", fi);
}

void RepoStateLogger::logAddFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    logAddOrModifyFile("add_file", filePath, modifiedAt, sizeInBytes, sha1);
}

void RepoStateLogger::logModifyFile(FileInfo* fi)
{
    logAddOrModifyFile("modify_file", fi);
}

void RepoStateLogger::logModifyFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    logAddOrModifyFile("modify_file", filePath, modifiedAt, sizeInBytes, sha1);
}

void RepoStateLogger::logAddOrModifyFile(QString operation, FileInfo* fi)
{
    logAddOrModifyFile(operation, fi->filePath(), fi->lastModified(),
                       fi->size(), fi->fingerPrint());
}

void RepoStateLogger::logAddOrModifyFile(QString operation,
                                             QString filePath,
                                             QDateTime modifiedAt,
                                             qint64 sizeInBytes,
                                             QString sha1)
{
    QStringList tokens;
    tokens << operation << filePath << modifiedAt.toString(Qt::ISODate)
           << QString::number(sizeInBytes) << sha1;
    addLogLine(tokens);
}

void RepoStateLogger::logRemoveFile(QString file_path)
{
    QStringList tokens;
    tokens << "remove_file" << file_path;
    addLogLine(tokens);
}

void RepoStateLogger::logRenameFile(QString file_path, QString new_file_path)
{
    QStringList tokens;
    tokens << "rename_file" << file_path << new_file_path;
    addLogLine(tokens);
}


void RepoStateLogger::addLogLine(QStringList tokens)
{
    m_logLines << tokens;
}