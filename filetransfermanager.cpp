#include "filetransfermanager.h"
#include <QStringList>
FileTransferManager::FileTransferManager(QObject *parent) :
    QObject(parent)
{
}

FileTransfer* FileTransferManager::copy(QString file_path, qint64 file_size)
{
    FileTransfer* ft = new FileTransfer(this, file_path, file_size);
    m_transfers.insert(ft);
    return ft;
}
