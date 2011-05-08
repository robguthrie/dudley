#include "repo.h"
#include "statelogger.h"
#include "synchronizer.h"
#include <QCoreApplication>

Repo::Repo(QObject *parent, QString path)
    :QObject(parent), m_path(path)
{
}

State* Repo::state() const
{
    return m_logger->state();
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
            qCritical() << "Cannot initialize logger";
        }
    }else{
        qCritical() << "Can't read data: " << m_path;
    }
    return false;
}

// what about passing a state object here instead?
// prolly dont need to pass a state obj actually.. the default seems to work well
// and returning a statediff object?
// i like the idea of returning the sd obj
// going via the logger actually serves no purpose at this point..
// except the logger knows about the objects easily



StateDiff Repo::detectChanges() const
{
    State* state = m_logger->state();
    StateDiff diff;
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
        if (!stored_fi.sameModifiedAtAndSize(qfi.lastModified(), qfi.size())){
            // file has changed on disk but filename is the same.
            // record the new values for the file
            diff.modifyFile(file_path, qfi.size(), qfi.lastModified(),
                                readFingerPrint(file_path));
        }
    }

    // scanning unknown files
    bool file_was_renamed;
    foreach(file_path, unknown_file_paths){
        file_was_renamed = false;
        FileInfo unknown_fi = readFileInfo(file_path);
        foreach(QString missing_file_path, missing_file_paths){
            QCoreApplication::processEvents();
            FileInfo missing_fi = state->fileInfoByFilePath(missing_file_path);
            if (missing_fi.isIdenticalTo(unknown_fi)){
                // this unknown file is actually a missing file renamed
                file_was_renamed = true;
                missing_file_paths.removeAll(missing_file_path);
                unknown_file_paths.removeAll(file_path);
                diff.renameFile(missing_file_path, file_path);
            }
        }

        // add the file as new to the collection
        if (!file_was_renamed){
            diff.addFile(unknown_fi.filePath(), unknown_fi.size(),
                              unknown_fi.modifiedAt(), unknown_fi.fingerPrint());
        }
    }

    // deleted sweep
    foreach(file_path, missing_file_paths){
        diff.removeFile(file_path);
    }
    return diff;
}

// then think about the reverse.. performChanges
// we pass a state diff object and expect the repo to
// preform the changes upon itself..
// it would preform each change
// foreach(op, stateops)
//   if action
//     each actions special stuff.. new repo private methods to match actions
//     plus have a testing mode where we try to detect if it would have failed
//     also check if we are preforming an action (such as delete) on a file in the dl_queue
//          if so then remove it from the download queue? or atleast mention this in debug
// downloads would be queued
// then the statediff would be applied to the state
void Repo::performChanges(StateDiff *state_diff, Synchronizer *synchronizer)
{
    /*
      so check that the changes can be performed..
      then queue up downloads, maybe pasing iodevices to synchronizer
      download to tmp folder in repo then rename when finished signal happpens.
      so for each AddFile,
        open a writable file,
        store the op and the iodevice* in m_incommingFiles hash
        ask syncer to download to iodevice and connect to success and failure signals
      */

    QHash<QString, StateOp> dl_queue;
    QList<StateOp> *state_ops = state_diff->stateOps();
    foreach(StateOp op, *state_ops){
        QString action = op.action();
        if ((action == "AddFile") || (action == "ModifyFile")){
            dl_queue[op.filePath()] = op;
        }else if (action == "RenameFile"){
            //rename file as instructed
        }else if (action == "RemoveFile"){
            //remove tha file
        }

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
    m["path"] = QVariant(this->path());
    m["type"] = QVariant(this->type());
    return m;
}
