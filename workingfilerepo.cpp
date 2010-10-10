#include "workingfilerepo.h"
#include <QCryptographicHash>
#include <QDir>
#include "output.h"

WorkingFileRepo::WorkingFileRepo(QObject *parent, QString path, QString name)
    :FileRepo(parent, path, name)
{
    m_log_path = m_path + "/.dudley/logs";
    m_state = new FileRepoState(this, m_log_path);
}


bool WorkingFileRepo::canReadData() const
{
    QDir dir;
    return dir.exists(m_path);
}

QString WorkingFileRepo::type() const
{
    return QString("WorkingFileRepo");
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
void WorkingFileRepo::updateState()
{
    Output::verbose(QString("building list of files"));

    QStringList found_files = filesOnDisk();

    // a missing file is in the collection but not on the disk
    // checking for files which have gone missing
    QStringList missing_file_paths = m_state->missingFilePaths(found_files);

    // checking for files which are unrecognised
    // an unknown file is on the disk but not in the collection
    QStringList unknown_found_file_paths = m_state->unknownFilePaths(found_files);

    // checking for known files
    QStringList known_found_file_paths = m_state->knownFilePaths(found_files);

    // scanning known files for modifications
    QString file_path;
    foreach(file_path, known_found_file_paths){
        // check if that file has been modified
        // to save overhead.. we dont call newFileInfo as that reads the sha1
        // and we only want to read the sha1 if the mtime has changed
        FileInfo *stored_fi = m_state->fileInfoByFilePath(file_path);
        QFileInfo qfi(m_path+'/'+file_path);
        if (!stored_fi->seemsIdenticalTo(qfi)){
            // file has changed on disk but filename is the same.
            // record the new values for the file
            m_state->modifyFile(file_path, qfi.lastModified(), qfi.size(),
                                readFingerPrint(file_path));
        }
    }

    // scanning unknown files
    foreach(file_path, unknown_found_file_paths){
        bool file_was_renamed = false;
        FileInfo *unknown_fi = newFileInfo(file_path);
        foreach(QString missing_file_path, missing_file_paths){
            FileInfo *missing_fi = m_state->fileInfoByFilePath(missing_file_path);
            if (missing_fi->isIdenticalTo(unknown_fi)){
                // this unknown file is actually a missing file renamed
                file_was_renamed = true;
                missing_file_paths.removeAll(missing_file_path);
                unknown_found_file_paths.removeAll(file_path);
                m_state->renameFile(missing_file_path, file_path);
            }
        }

        // add the file as new to the collection
        if (!file_was_renamed) m_state->addFile(unknown_fi);
    }

    // deleted sweep
    foreach(file_path, missing_file_paths) m_state->removeFile(file_path);
}

bool WorkingFileRepo::hasFile(FileInfo fileInfo) const
{
    //need to actually check he disk here..
    // do we need to check the state at all?
    // i dont get why?
    //    return m_state->containsFileInfo(fileInfo);
    return QFile::exists ( m_path +"/"+ fileInfo.filePath());
}

QIODevice* WorkingFileRepo::getFile(FileInfo* fileInfo)
{
    QFile *f = new QFile(m_path +"/"+ fileInfo->filePath());
    if (f->open(QIODevice::ReadOnly)){
        return f;
    }else{
        return 0;
    }
}

// private functions

QStringList WorkingFileRepo::filesOnDisk()
{
    QStringList found_files;
    findAllFiles(m_path, &found_files);
    return found_files;
}

void WorkingFileRepo::findAllFiles(QString path, QStringList *found_files)
{
    // first grab the list of files
    Output::verbose(path);
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList fileNames = dir.entryList();
    QString fileName;
    foreach(fileName, fileNames){
        found_files->append(relativeFilePath(path+'/'+fileName));
    }

    // then recurse down into every directory

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList dirs = dir.entryList();
    for (int i = 0; i < dirs.size(); ++i) {
        findAllFiles(path+"/"+dirs[i], found_files);
    }
}

QString WorkingFileRepo::relativeFilePath(QString filePath){
    if (filePath.startsWith(m_path+'/')){
        return filePath.remove(0, m_path.length()+1);
    }else{
        return filePath;
    }
}

FileInfo* WorkingFileRepo::newFileInfo(QString filePath)
{
    QFileInfo *qfi = new QFileInfo(m_path+"/"+filePath);
    return new FileInfo(filePath, qfi->lastModified(),
                        qfi->size(), readFingerPrint(filePath));
}

QString WorkingFileRepo::readFingerPrint(QString filePath)
{
    QFile file(m_path+'/'+filePath);
    Output::info(QString("reading fingerprint of ").append(filePath));
    QCryptographicHash hash(QCryptographicHash::Sha1);
    if (file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
            while (!file.atEnd())
                hash.addData(file.read(10485760));
            return hash.result().toHex();
    }else{
        return "failed to open file for fingerprint";
    }
}
