#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDateTime>
#include <QString>
#include <QFileInfo>
#include "mimetypefinder.h"


class RepoState;

class FileInfo
{

    static MimeTypeFinder mimeTypeFinder;
public:
    FileInfo();
    FileInfo(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    FileInfo(const FileInfo &f);
    void update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    bool isIdenticalTo(FileInfo *fi);
    bool seemsIdenticalTo(QFileInfo q);

    QDateTime lastModified() const;
    QString fingerPrint() const;
    QString filePath() const;
    QString fileName() const;
    QString toString() const;
    QString mimeType() const;
    QString humanSize() const;
    qint64 size() const;
    void rename(QString newFilePath);

private:
    QDateTime m_modifiedAt; // last modification date of file
    qint64 m_sizeInBytes;
    QString m_filePath;
    QString m_sha1;
};

#endif // FILEINFO_H
