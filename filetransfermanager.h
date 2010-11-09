#ifndef FILETRANSFERMANAGER_H
#define FILETRANSFERMANAGER_H

#include <QObject>
#include <QSet>
#include <filetransfer.h>
class FileTransferManager : public QObject
{
    Q_OBJECT
public:
    explicit FileTransferManager(QObject *parent = 0);

    FileTransfer* copy(QString file_path, qint64 file_size = -1);

signals:

public slots:

protected:
    QSet<FileTransfer*> m_transfers;
};

#endif // FILETRANSFERMANAGER_H
