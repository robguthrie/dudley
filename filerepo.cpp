#include "filerepo.h"
#include "filerepostatelogger.h"
#include "output.h"
FileRepo::FileRepo(QObject *parent, QString path, QString name)
    :QObject(parent), m_path(path), m_name(name)
{
    // read the settings
}

FileRepoState* FileRepo::state()
{
    return m_state;
}

QString FileRepo::path()
{
    return m_path;
}

QString FileRepo::log_path()
{
    return m_log_path;
}

QString FileRepo::name()
{
    return m_name;
}

bool FileRepo::hasFileInfoByFilePath(QString file_name) const
{
    return m_state->containsFilePath(file_name);
}

bool FileRepo::hasFileInfoByFingerPrint(QString finger_print) const
{
    return m_state->containsFingerPrint(finger_print);
}


FileInfo* FileRepo::fileInfoByFingerPrint(QString finger_print) const
{
    return m_state->fileInfoByFingerPrint(finger_print);
}

FileInfo* FileRepo::fileInfoByFilePath(QString file_path) const
{
    return m_state->fileInfoByFilePath(file_path);
}

// read actual values from the instance
QMap<QString, QVariant> FileRepo::settings()
{
    /*
      name
      path
      type
      logs_dir?
      */
    QMap<QString, QVariant> m;
    m["name"] = QVariant(this->name());
    m["path"] = QVariant(this->path());
    m["type"] = QVariant(this->type());
    return m;
}

bool FileRepo::isReady() const
{
    return this->canReadData() && this->m_state->logger()->isReady();
}

bool FileRepo::initialize()
{
    if (this->canReadData()){
        if (m_state->logger()->initialize()){
            return true;
        }else{
            Output::error("Cannot initialize history logger");
        }
    }else{
        Output::error(QString("Can't read data: ").append(m_path));
    }
    return false;
}
