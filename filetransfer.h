#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QIODevice>
#include "fileinfo.h"
#include <QStringList>
class Repo;
class FileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FileTransfer(QObject *parent, QString file_path, qint64 file_size = -1);


    QString     sourceName() const;
    QString     destName() const;
    QString     filePath() const;
    QIODevice*  sourceDevice() const;
    QIODevice*  destDevice() const;

    void        setSource(QString source_name, QIODevice* device = 0);
    void        setDest(QString dest_name, QIODevice* device = 0);
    void        setDestRepo(Repo* repo);


    bool        start();
    void        setFinished();
    qint64      bytesWritten() const;
    QStringList errors() const;
    void        printErrors() const;
    QString     status() const;
    QString     statusLine() const;
    bool        isComplete() const;
    bool        isFinished() const;

signals:
    void complete(qint64 file_size);
    void failed();
    void finished();

public slots:
    void processReadyRead();
    void processBytesWritten(qint64 bw);
    void setFileSize(qint64 size);

private:
    bool        everythingIsOk();
    void        checkIfFinished();
    QString     m_sourceName;
    QIODevice*  m_sourceDevice;

    QString     m_destName;
    QIODevice*  m_destDevice;
    Repo*       m_destRepo;

    QString     m_filePath;
    qint64      m_fileSize;
    qint64      m_bytesWritten;
    QStringList m_errors;
    bool        m_complete;
    bool        m_finished;
};

#endif // FILETRANSFER_H
