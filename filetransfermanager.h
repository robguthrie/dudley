#ifndef FILETRANSFERMANAGER_H
#define FILETRANSFERMANAGER_H

#include <QObject>
#include <filetransfer.h>
class FileTransferManager : public QObject
{
    Q_OBJECT
public:
    explicit FileTransferManager(QObject *parent = 0);
//    FileTransfer* copy(FileRepo* source);
signals:

public slots:

};

#endif // FILETRANSFERMANAGER_H
