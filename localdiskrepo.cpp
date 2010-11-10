#include "repo.h"
#include "repostate.h"
#include "localdiskrepo.h"
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDir>
#include "output.h"
#include "filetransfer.h"

LocalDiskRepo::LocalDiskRepo(QObject *parent, QString path, QString name)
    :Repo(parent, path, name)
{
    m_log_path = m_path + "/.dudley/logs";
    m_state = new RepoState(this, m_log_path);
}


bool LocalDiskRepo::canReadData() const
{
    QDir dir;
    return dir.exists(m_path);
}

QString LocalDiskRepo::type() const
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
void LocalDiskRepo::updateState(bool commit_changes)
{
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
        QCoreApplication::processEvents();
        // check if that file has been modified
        // to save effort we dont call newFileInfo as that reads the sha1
        // and we only want to read the sha1 if the mtime has changed
        FileInfo *stored_fi = m_state->fileInfoByFilePath(file_path);
        QFileInfo qfi(m_path+'/'+file_path);
        if (!stored_fi->seemsIdenticalTo(qfi)){
            // file has changed on disk but filename is the same.
            // record the new values for the file
            m_state->modifyFile(file_path, qfi.size(), qfi.lastModified(),
                                readFingerPrint(file_path));
        }
    }

    // scanning unknown files
    foreach(file_path, unknown_found_file_paths){
        bool file_was_renamed = false;
        FileInfo *unknown_fi = newFileInfo(file_path);
        foreach(QString missing_file_path, missing_file_paths){
            QCoreApplication::processEvents();
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

    if (commit_changes){
        m_state->commitChanges();
    }
}

bool LocalDiskRepo::hasFile(FileInfo* file_info) const
{
    // need to actually check the disk here..
    return QFile::exists( m_path +"/"+ file_info->filePath());
}

QIODevice* LocalDiskRepo::getFile(FileInfo* fileInfo)
{
    QFile *f = new QFile(m_path +"/"+ fileInfo->filePath());
    if (f->open(QIODevice::ReadOnly)){
        return f;
    }else{
        return 0;
    }
}

QIODevice* LocalDiskRepo::putFile(QString file_path)
{
    QFile* f = new QFile(this->temporaryFilePath(file_path));
    if (f->open(QIODevice::WriteOnly | QIODevice::Truncate)){
        return f;
    }else{
        g_log->error("Could not open temporaryFileDevice on "+this->name()+": "+this->temporaryFilePath(file_path));
        return 0;
    }
}

void LocalDiskRepo::putFileComplete(QIODevice* device, QString file_path)
{
    g_log->debug("saving file");
    disconnect(device, SIGNAL(aboutToClose()), this, SLOT(putFileFailed()));
    QFile* f = (QFile*) device;
    if (!QFile::exists(absoluteFilePath(file_path))){
        g_log->debug("renaming writebuffer and closing");
        f->rename(absoluteFilePath(file_path));
    }else{
        // keeping as temporary
        g_log->error("file with file_path:"+file_path+" already exists");
        f->close();
   }
}

void LocalDiskRepo::putFileFailed(QIODevice *device)
{
    g_log->warning("deleteing a writable buffer - presumably a put file has failed");
    QFile* f;
    if (!device){
        f = (QFile*) sender();
    }else{
        f = (QFile*) device;
    }

    f->remove();
    f->deleteLater();
}

QString LocalDiskRepo::absoluteFilePath(QString file_path)
{
    return m_path+"/"+file_path;
}

QString LocalDiskRepo::temporaryFilePath(QString file_path)
{
    return m_path+"/"+file_path+".part";
}

QStringList LocalDiskRepo::filesOnDisk()
{
    QStringList found_files;
    findAllFiles(m_path, &found_files);
    return found_files;
}

void LocalDiskRepo::findAllFiles(QString path, QStringList *found_files)
{
    QCoreApplication::processEvents();
    // first grab the list of files
    g_log->verbose(path);
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList fileNames = dir.entryList();
    QString fileName;
    foreach(fileName, fileNames){
        // .part is the extention for a file we are still writing
        // we dont wanna know about these files
        if (!fileName.endsWith(".part")){
            found_files->append(relativeFilePath(path+'/'+fileName));
        }
    }

    // then recurse down into every directory

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList dirs = dir.entryList();
    for (int i = 0; i < dirs.size(); ++i) {
        findAllFiles(path+"/"+dirs[i], found_files);
    }
}

QString LocalDiskRepo::relativeFilePath(QString filePath){
    if (filePath.startsWith(m_path+'/')){
        return filePath.remove(0, m_path.length()+1);
    }else{
        return filePath;
    }
}

FileInfo* LocalDiskRepo::newFileInfo(QString filePath)
{
    QFileInfo *qfi = new QFileInfo(m_path+"/"+filePath);
    return new FileInfo(this, filePath, qfi->size(),
                        qfi->lastModified(), readFingerPrint(filePath));
}

QString LocalDiskRepo::readFingerPrint(QString filePath)
{
    QFile file(m_path+'/'+filePath);
    g_log->info(QString("reading fingerprint of ").append(filePath));
    QCryptographicHash hash(QCryptographicHash::Sha1);
    file.open(QIODevice::ReadOnly | QIODevice::Unbuffered);
    return readFingerPrint(&file);
}

QString LocalDiskRepo::readFingerPrint(QFile* d)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    if (d->isOpen() && d->isReadable()) {
        while (!d->atEnd()){
            hash.addData(d->read(256*1024));
            QCoreApplication::processEvents();
        }
        return hash.result().toHex();
    }else{
        g_log->error("could not open device to read fingerprint");
        return "";
    }
}
