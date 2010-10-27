#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QIODevice>
#include "fileinfo.h"
#include <QStringList>
class FileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FileTransfer(QObject *parent, QString    source_name,
                                           QIODevice* source_file,
                                           QString   dest_name,
                                           QIODevice* dest_file,
                                           FileInfo*   file_info);

    QString     sourceName() const;
    QString     destName() const;
    QIODevice*  sourceFile() const;
    QIODevice*  destFile() const;
    qint64      bytesWritten() const;
    FileInfo*   fileInfo() const ;
    QStringList errors() const;
    void        printErrors() const;
    QString     status() const;
    QString     statusLine() const;
    bool        isComplete() const;
    bool        isFinished() const;

signals:
    void finished();

public slots:
    void processReadyRead();
    void processSourceComplete(qint64 contentBytesReceived);
    void processAboutToClose();

private:
    bool        everythingIsOk();
    QString     m_sourceName;
    QIODevice*  m_sourceFile;

    QString     m_destName;
    QIODevice*  m_destFile;
    FileInfo*   m_fileInfo;

    qint64      m_bytesWritten;
    QStringList m_errors;
    bool        m_complete;
    bool        m_finished;
};

#endif // FILETRANSFER_H
