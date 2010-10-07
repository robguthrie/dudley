#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDateTime>
#include <QString>
#include <QFileInfo>
#include "mimetypefinder.h"


class FileRepoState;

class FileInfo
{

    static MimeTypeFinder mimeTypeFinder;
public:
    FileInfo(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    bool isIdenticalTo(FileInfo *fi);
    bool seemsIdenticalTo(QFileInfo q);

    QDateTime lastModified();
    QString fingerPrint();
    QString filePath();
    QString fileName();
    QString toString();
    QString mimeType();
    QString humanSize();
    qint64 size();
    void rename(QString newFilePath);

private:
    QDateTime m_modifiedAt; // last modification date of file
    qint64 m_sizeInBytes;
    QString m_filePath;
    QString m_sha1;
};

#endif // FILEINFO_H
