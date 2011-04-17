#include "repo.h"
#include "state.h"
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
    m_logger = new StateLogger(this, m_log_path);
}

bool LocalDiskRepo::canReadData() const
{
    QDir dir;
    return dir.exists(m_path);
}

QString LocalDiskRepo::type() const
{
    return QString("LocalDisk");
}


bool LocalDiskRepo::fileExists(FileInfo* file_info) const
{
    // need to actually check the disk here..
    return QFile::exists( m_path +"/"+ file_info->filePath());
}

QIODevice* LocalDiskRepo::getFile(FileInfo* fileInfo)
{
    QFile *f = new QFile(m_path +"/"+ fileInfo->filePath());
    if (f->open(QIODevice::ReadOnly)){
        return f;
    }else return 0;
}

QIODevice* LocalDiskRepo::putFile(QString file_path)
{
    QFile* f = new QFile(this->temporaryFilePath(file_path));
    if (f->open(QIODevice::WriteOnly | QIODevice::Truncate)){
        return f;
    }else{
        qCritical("Could not open temporaryFileDevice on "+this->name()+": "+this->temporaryFilePath(file_path));
        return 0;
    }
}

void LocalDiskRepo::putFileComplete(QIODevice* device, QString file_path)
{
    qDebug("saving file");
    disconnect(device, SIGNAL(aboutToClose()), this, SLOT(putFileFailed()));
    QFile* f = (QFile*) device;
    if (!QFile::exists(absoluteFilePath(file_path))){
        qDebug("renaming writebuffer and closing");
        f->rename(absoluteFilePath(file_path));
    }else{
        // keeping as temporary
        qCritical("file with file_path:"+file_path+" already exists");
        f->close();
   }
}

void LocalDiskRepo::putFileFailed(QIODevice *device)
{
    g_log->warning("deleting a writable buffer - presumably a put file has failed");
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

QStringList LocalDiskRepo::readFilePaths()
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

FileInfo* LocalDiskRepo::readFileInfo(QString filePath)
{
    QFileInfo *qfi = new QFileInfo(m_path+"/"+filePath);
    return new FileInfo(this, filePath, qfi->size(),
                        qfi->lastModified(), readFingerPrint(filePath));
}

FileInfo* LocalDiskRepo::readFileInfoCheap(QString filePath)
{
    QFileInfo *qfi = new QFileInfo(m_path+"/"+filePath);
    return new FileInfo(this, filePath, qfi->size(), qfi->lastModified(), "");
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
        qCritical("could not open device to read fingerprint");
        return "";
    }
}
