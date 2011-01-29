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

/*
    updateState updates the m_state object to represent the current state

    first, determine missing files
    then split found files (ie: the whole list) into known and unknown files
    then split known into modified and unchanged
    at this stage we wanna read sha1's for modified and unknown files
    then split unknown into renamed (removing them from missing) and new
    then turn remaining missing files into deleted

*/
void Repo::updateState()
{
    State* state = m_logger->state();
    StateDiff* diff = m_logger->diff();

    QStringList found_files = this->readFilePaths();

    // a missing file is in the collection but not on the disk
    // checking for files which have gone missing
    QStringList missing_file_paths = state->missingFilePaths(found_files);

    // checking for files which are unrecognised
    // an unknown file is on the disk but not in the collection
    QStringList unknown_found_file_paths = state->unknownFilePaths(found_files);

    // checking for known files
    QStringList known_found_file_paths = state->knownFilePaths(found_files);

    // scanning known files for modifications
    QString file_path;
    foreach(file_path, known_found_file_paths){
        QCoreApplication::processEvents();
        // check if that file has been modified
        // to save effort we dont call newFileInfo as that reads the sha1
        // and we only want to read the sha1 if the mtime has changed
        FileInfo *stored_fi = state->fileInfoByFilePath(file_path);
        QFileInfo qfi(m_path+'/'+file_path);
        if (!stored_fi->seemsIdenticalTo(qfi.lastModified(), qfi.size())){
            // file has changed on disk but filename is the same.
            // record the new values for the file
            diff->modifyFile(file_path, qfi.size(), qfi.lastModified(),
                                readFingerPrint(file_path));
        }
    }

    // scanning unknown files
    foreach(file_path, unknown_found_file_paths){
        bool file_was_renamed = false;
        FileInfo *unknown_fi = readFileInfo(file_path);
        foreach(QString missing_file_path, missing_file_paths){
            QCoreApplication::processEvents();
            FileInfo *missing_fi = state->fileInfoByFilePath(missing_file_path);
            if (missing_fi->isIdenticalTo(unknown_fi)){
                // this unknown file is actually a missing file renamed
                file_was_renamed = true;
                missing_file_paths.removeAll(missing_file_path);
                unknown_found_file_paths.removeAll(file_path);
                diff->renameFile(missing_file_path, file_path);
            }
        }

        // add the file as new to the collection
        if (!file_was_renamed){
            diff->addFile(unknown_fi);
        }

    // deleted sweep
    foreach(file_path, missing_file_paths){
        diff->removeFile(file_path);
    }
}
