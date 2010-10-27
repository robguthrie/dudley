#include "filetransfermanager.h"
#include <QStringList>
FileTransferManager::FileTransferManager(QObject *parent) :
    QObject(parent)
{
}

FileTransfer* FileTransferManager::copy(QString source_name, QIODevice* source_file,
                                        QString dest_name, QIODevice* dest_file, FileInfo* file_info)
{
    FileTransfer* ft = new FileTransfer(this, source_name, source_file, dest_name, dest_file, file_info);
    return ft;
}
