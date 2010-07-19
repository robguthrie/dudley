#include "filerepostatelogger.h"
#include <QDir>
#include <iostream>
#include "output.h"
#include <QCryptographicHash>

// logger is a file state history log
FileRepoStateLogger::FileRepoStateLogger(QString logsDir)
{
    m_logsDir = logsDir;
}

bool FileRepoStateLogger::initialize()
{
    QDir dir;
    return dir.mkpath(m_logsDir);
}

bool FileRepoStateLogger::isReady() const
{
    QDir dir;
    return dir.exists(m_logsDir);
}

QString FileRepoStateLogger::logsDir() const
{
    return m_logsDir;
}

// given a state object.. play the list of file operations stored
// across each log file representing the history, (they each represent a commit)
// so that the state object represents the last
// known state of the directory we track
FileRepoState FileRepoStateLogger::loadState()
{
    // foreach .log file in the directory play it into the state
    FileRepoState* state = new FileRepoState();
    loadState(state);
    return (*state);
}

// given a state object.. play the list of file operations stored
// across each log file representing the history, (they each represent a commit)
// so that the state object represents the last
// known state of the directory we track
void FileRepoStateLogger::loadState(FileRepoState* state)
{
    // foreach .log file in the directory play it into the state
    state->stopLoggingChanges();
    QDir dir(m_logsDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i) {
        QString filename = list.at(i);
        if (filename.endsWith(".log")) {
            readLogFile(m_logsDir+"/"+filename, state);
        }
    }
    state->logChanges(this);
}

/* open a logfile and read it. line by line. thus startign the process of
   updating the in-memory representation of the state

   within this you will see calls to state->dosomethingFileSilent
   the silent is to prevent this state building process to be the same as
   recording new state
*/
void FileRepoStateLogger::readLogFile(QString logFilePath, FileRepoState* state)
{
    QString line;
    QFile file(logFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Output::info(QString("Reading Logfile: ").append(logFilePath));

        QTextStream in(&file);
        while (!in.atEnd()){

            //  operation, filepath
            //  add_file, filepath, mtime(ISO), size, sha1
            //  remove_file, filepath
            //  move_file, filepath, newfilepath
            QString line = in.readLine();
            QStringList parts = line.split(',');
            // we ignore the line timestamp.. it will be used later..
            QString operation(parts[0].trimmed());
            QString filePath = parts[1].trimmed().replace("DUDLEYCOMMA", ",");

            if ((operation == "add_file") || (operation == "modify_file")){
                QDateTime modifiedAt = QDateTime::fromString(parts[2].trimmed(), Qt::ISODate);
                qint64 sizeInBytes = parts[3].trimmed().toULongLong();
                QString sha1 = parts[4].trimmed();
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
                QString newFilePath = parts[2].trimmed().replace("DUDLEYCOMMA", ",");
                state->renameFile(filePath, newFilePath);
            }
        }
        file.close();
    }else{
        Output::error(QString("Could not read Logfile: ").append(logFilePath));
    }
}

// flush the staged operations to a commit log file
// should probably do the operation, fileInfo* to log line here..
void FileRepoStateLogger::writeLogFile()
{
    if (m_logLines.size() > 0){
        if (!QFile::exists(m_logsDir)){
            QDir dir;
            dir.mkdir(m_logsDir);
        }
        // first get the hash of the logfile
        QByteArray log_data = m_logLines.join("\n").toUtf8();
        QByteArray hash = QCryptographicHash::hash(log_data, QCryptographicHash::Sha1);

        QFile file(m_logsDir+"/"+QDateTime::currentDateTime().toString(Qt::ISODate)+"_"+QString(hash)+".log");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Append)){
            QTextStream out(&file);
            out << log_data;
            Output::info(QString::number(m_logLines.size()).append(" lines appended to logfile"));
            file.close();
            m_logLines.empty();
        }else{
            Output::error("could not open history.log for writing");
        }
    }
}

// show the operations staged to be written to a history commit file.
void FileRepoStateLogger::printLogFile()
{
    if (m_logLines.size() > 0){
        std::cout << qPrintable(m_logLines.join("\n")) << std::endl;
    }
}

void FileRepoStateLogger::logAddFile(FileInfo* fi)
{
    logAddOrModifyFile("add_file", fi);
}

void FileRepoStateLogger::logAddFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    logAddOrModifyFile("add_file", filePath, modifiedAt, sizeInBytes, sha1);
}

void FileRepoStateLogger::logModifyFile(FileInfo* fi)
{
    logAddOrModifyFile("modify_file", fi);
}

void FileRepoStateLogger::logModifyFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    logAddOrModifyFile("modify_file", filePath, modifiedAt, sizeInBytes, sha1);
}

void FileRepoStateLogger::logAddOrModifyFile(QString operation, FileInfo* fi)
{
    logAddOrModifyFile(operation, fi->filePath(), fi->lastModified(),
                       fi->size(), fi->fingerPrint());
}

void FileRepoStateLogger::logAddOrModifyFile(QString operation,
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

void FileRepoStateLogger::logRemoveFile(QString file_path)
{
    QStringList tokens;
    tokens << "remove_file" << file_path;
    addLogLine(tokens);
}


void FileRepoStateLogger::logRenameFile(QString file_path, QString new_file_path)
{
    QStringList tokens;
    tokens << "rename_file" << file_path << new_file_path;
    addLogLine(tokens);
}

void FileRepoStateLogger::addLogLine(QStringList tokens)
{
   tokens.replaceInStrings(",", "DUDLEYCOMMA");
   m_logLines << tokens.join(",");
}
