#include "repo.h"
#include "statelogger.h"
#include "output.h"
Repo::Repo(QObject *parent, QString path, QString name)
    :QObject(parent), m_path(path), m_name(name)
{
    // read the settings

}

bool Repo::initialize()
{
    if (this->canReadData()){
        if (m_logger->initialize()){
            return true;
        }else{
            g_log->error("Cannot initialize history logger");
        }
    }else{
        g_log->error(QString("Can't read data: ").append(m_path));
    }
    return false;
}

StateLogger* Repo::logger()
{
    return m_logger;
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
    return this->canReadData() && m_logger && m_logger->isReady();
}
