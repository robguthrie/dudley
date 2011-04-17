#include "repo.h"
#include "statelogger.h"
#include "output.h"
Repo::Repo(QObject *parent, QString path, QString name)
    :QObject(parent), m_path(path), m_name(name)
{
}

QString Repo::name() const
{
    return m_name;
}

QString Repo::path() const
{
    return m_path;
}

QString Repo::logPath()
{
    return m_log_path;
}

bool Repo::isReady() const
{
    return canReadData() && m_logger && m_logger->isReady();
}

bool Repo::initialize()
{
    if (canReadData()){
        if (m_logger->initialize()){
            return true;
        }else{
            qCritical("Cannot initialize logger");
        }
    }else{
        qCritical(QString("Can't read data: ").append(m_path));
    }
    return false;
}

void Repo::detectChanges()
{
    QStringList found_files = readFilePaths();

    // checking for known files
    // known_file_paths will hold a list of all file_paths in the store and the state
    QStringList known_file_paths = state->knownFilePaths(found_files);

    // checking for files which are unrecognised
    // an unknown file is on the disk but not in the collection
    QStringList unknown_file_paths = state->unknownFilePaths(found_files);

    // a missing file is in the state but not in the repo
    // check for files which have gone missing
    QStringList missing_file_paths = state->missingFilePaths(found_files);



    // scanning known files for modifications
    QString file_path;
    foreach(file_path, known_file_paths){
        QCoreApplication::processEvents();
        // check if that file has been modified
        // to save effort we dont call newFileInfo as that reads the sha1
        // and we only want to read the sha1 if the mtime has changed
        FileInfo stored_fi = state->fileInfoByFilePath(file_path);
        QFileInfo qfi(m_path+'/'+file_path);
        if (!stored_fi->sameModifiedAtAndSize(qfi.lastModified(), qfi.size())){
            // file has changed on disk but filename is the same.
            // record the new values for the file
            m_logger->modifyFile(file_path, qfi.size(), qfi.lastModified(),
                                readFingerPrint(file_path));
        }
    }

    // scanning unknown files
    bool file_was_renamed;
    foreach(file_path, unknown_file_paths){
        file_was_renamed = false;
        FileInfo *unknown_fi = readFileInfo(file_path);
        foreach(QString missing_file_path, missing_file_paths){
            QCoreApplication::processEvents();
            FileInfo *missing_fi = state->fileInfoByFilePath(missing_file_path);
            if (missing_fi->isIdenticalTo(unknown_fi)){
                // this unknown file is actually a missing file renamed
                file_was_renamed = true;
                missing_file_paths.removeAll(missing_file_path);
                unknown_found_file_paths.removeAll(file_path);
                m_logger->renameFile(missing_file_path, file_path);
            }
        }

        // add the file as new to the collection
        if (!file_was_renamed){
            m_logger->addFile(unknown_fi);
        }
    }

    // deleted sweep
    foreach(file_path, missing_file_paths){
        m_logger->removeFile(file_path);
    }
}

StateLogger* Repo::logger()
{
    return m_logger;
}

// read actual values from the instance
QMap<QString, QVariant> Repo::settings()
{
    QMap<QString, QVariant> m;
    m["name"] = QVariant(this->name());
    m["path"] = QVariant(this->path());
    m["type"] = QVariant(this->type());
    return m;
}
