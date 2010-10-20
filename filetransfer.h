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
    explicit FileTransfer(QObject *parent, QObject*   source_parent,
                                           QIODevice* source_file,
                                           QObject*   dest_parent,
                                           QIODevice* dest_file,
                                           FileInfo*   file_info);

    QObject*    sourceParent() const;
    QObject*    destParent() const;
    QIODevice*  sourceFile() const;
    QIODevice*  destFile() const;
    qint64      bytesWritten() const;
    FileInfo*   fileInfo() const ;
    QStringList errors() const;
    void        printErrors() const;
    bool        isComplete() const;

signals:
    void finished();

public slots:
    void processReadyRead();
    void processSourceComplete(qint64 contentBytesReceived);
    void processAboutToClose();

private:
    QObject*    m_sourceParent;
    QIODevice*  m_sourceFile;

    QObject*    m_destParent;
    QIODevice*  m_destFile;
    FileInfo*   m_fileInfo;

    qint64      m_bytesWritten;
    QStringList m_errors;
    bool        m_complete;
};

#endif // FILETRANSFER_H
