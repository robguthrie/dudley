#include "filetransfermanager.h"
#include <QStringList>
FileTransferManager::FileTransferManager(QObject *parent) :
    QObject(parent)
{
}

FileTransfer* FileTransferManager::copy(QObject* source_parent,
                                        QIODevice* source_file,
                                        QObject* dest_parent,
                                        QIODevice* dest_file,
                                        FileInfo* file_info)
{
    FileTransfer* ft = new FileTransfer(this, source_parent, source_file, dest_parent, dest_file, file_info);
    if (ft->errors().isEmpty()) {
        return ft;
    }else{
        return 0;
    }
}
