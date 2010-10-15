#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QIODevice>
class Repo;
class FileInfo;

class FileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FileTransfer(QObject *parent = 0);

signals:
    void finished();

public slots:

private:
    FileInfo  *m_fileInfo;
    Repo  *m_sourceRepo;
    Repo  *m_destRepo;
    QIODevice *m_sourceDevice;
    QIODevice *m_destDevice;
};

#endif // FILETRANSFER_H
