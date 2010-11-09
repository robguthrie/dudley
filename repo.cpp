#include "repo.h"
#include "repostatelogger.h"
#include "output.h"
Repo::Repo(QObject *parent, QString path, QString name)
    :QObject(parent), m_path(path), m_name(name)
{
    // read the settings
}

bool Repo::initialize()
{
    if (this->canReadData()){
        if (m_state->logger()->initialize()){
            return true;
        }else{
            g_log->error("Cannot initialize history logger");
        }
    }else{
        g_log->error(QString("Can't read data: ").append(m_path));
    }
    return false;
}

RepoState* Repo::state()
{
    return m_state;
}

QString Repo::path() const
{
    return m_path;
}

QString Repo::logPath()
{
    return m_log_path;
}

QString Repo::name() const
{
    return m_name;
}

FileInfo* Repo::fileInfoByFingerPrint(QString finger_print) const
{
    return m_state->fileInfoByFingerPrint(finger_print);
}

FileInfo* Repo::fileInfoByFilePath(QString file_path) const
{
    return m_state->fileInfoByFilePath(file_path);
}

// read actual values from the instance
QMap<QString, QVariant> Repo::settings()
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

bool Repo::isReady() const
{
    return this->canReadData() && this->m_state->logger()->isReady();
}

