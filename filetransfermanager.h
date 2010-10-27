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
    FileTransfer* copy(QString source_name, QIODevice* source_file,
                       QString dest_name, QIODevice* dest_file, FileInfo* file_info);
signals:

public slots:

protected:
    QSet<FileTransfer*> m_transfers;
};

#endif // FILETRANSFERMANAGER_H
