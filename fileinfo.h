#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDateTime>
#include <QString>
#include <QFileInfo>
#include "mimetypefinder.h"


class State;

class FileInfo
{
    static MimeTypeFinder mimeTypeFinder;
public:
    FileInfo();
    FileInfo(QString filePath, qint64 size = -1);
    FileInfo(QString filePath, qint64 size, QDateTime modifiedAt, QString sha1);

    QString   filePath()     const;
    QString   fileName()     const;
    qint64    size()         const;
    QString   fingerPrint()  const;
    QDateTime modifiedAt() const;
    QString   mimeType()     const;
    QString   toString()     const;

    bool isIdenticalTo(FileInfo fi) const;
    bool sameModifiedAtAndSize(QDateTime modifiedAt, qint64 size) const;

public:
    void update(qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void rename(QString newFilePath);
    void setFilePath(QString file_path);
    void setSize(qint64 size);
    void setFingerPrint(QString fingerprint);
    void setLastModified(QDateTime date_time);


private:
    QDateTime m_modifiedAt; // last modification date of file
    qint64 m_size;
    QString m_filePath;
    QString m_sha1;
};

QByteArray humanSize(qint64 n);
#endif // FILEINFO_H
