#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDateTime>
#include <QString>
#include <QFileInfo>
#include "mimetypefinder.h"


class State;

class FileInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
    Q_PROPERTY(qint64 size READ size WRITE setSize)
    Q_PROPERTY(QString fingerPrint READ fingerPrint WRITE setFingerPrint)
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified)
    Q_PROPERTY(QString mimeType READ mimeType)

    static MimeTypeFinder mimeTypeFinder;
public:
    FileInfo(QObject* parent = 0);
    FileInfo(QObject* parent, QString filePath, qint64 sizeInBytes = -1);
    FileInfo(QObject* parent, QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    FileInfo(const FileInfo &f);

    QString   filePath()     const;
    QString   fileName()     const;
    qint64    size()         const;
    QString   fingerPrint()  const;
    QDateTime lastModified() const;
    QString   mimeType()     const;
    QString   toString()     const;

    bool isIdenticalTo(FileInfo *fi);
    bool seemsIdenticalTo(QFileInfo q);

public slots:
    void update(qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void rename(QString newFilePath);
    void setFilePath(QString file_path);
    void setSize(qint64 size);
    void setFingerPrint(QString fingerprint);
    void setLastModified(QDateTime date_time);


private:
    QDateTime m_modifiedAt; // last modification date of file
    qint64 m_sizeInBytes;
    QString m_filePath;
    QString m_sha1;
};

#endif // FILEINFO_H
