#include "repo.h"
#include "repostatelogger.h"
#include "output.h"
Repo::Repo(QObject *parent, QString path, QString name)
    :QObject(parent), m_path(path), m_name(name)
{
    // read the settings
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

bool Repo::hasFileInfoByFilePath(QString file_name) const
{
    return m_state->containsFilePath(file_name);
}

bool Repo::hasFileInfoByFingerPrint(QString finger_print) const
{
    return m_state->containsFingerPrint(finger_print);
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

bool Repo::initialize()
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

QIODevice* Repo::putFile(const FileInfo &fileInfo)
{
    // start a temporary device to receive the file with
    // when we have fileInfo->size bytes, we can close it

    QIODevice* f = this->incommingFileDevice(fileInfo);
    if (f && f->isOpen() && f->isWritable()){
        m_incommingFiles.insert(f, fileInfo);
        connect(f, SIGNAL(aboutToClose()), this, SLOT(putFileAboutToClose()));
        return f;
    }else{
        Output::error("temporary file device is null or not writable");
        return 0;
    }
}

void Repo::putFileAboutToClose()
{
    QIODevice* file = (QIODevice*) sender();
    if (!m_incommingFiles.contains(file)){
        Output::error("unknown file (not in m_incommingFiles) about to close.. weird");
    }else{
        FileInfo file_info = m_incommingFiles.value(file);
        m_incommingFiles.remove(file);
        Output::debug("FileRepo::putFileAboutToClose file seems complete in size");
        this->putFileFinished(file_info, file);
    }
}