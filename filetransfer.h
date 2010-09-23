#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QtCore>
#include "fileinfo.h"
#include "filerepo.h"
class FileTransfer : public QObject
{
    Q_OBJECT
public:
    FileTransfer(FileRepo* source_repo, FileRepo* dest_repo, FileInfo* fi);

public slots:
    void stop();
    void start();

private slots:
    void processReadyRead();

signals:
    void progress(qint64, qint64);
    void finished();

private:
    FileInfo* m_fileInfo;
    QIODevice* m_source;
    QIODevice* m_dest;
    FileRepo* m_sourceRepo;
    FileRepo* m_destRepo;
    bool m_go = false;
};

#endif // FILETRANSFER_H
