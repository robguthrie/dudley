#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDateTime>
#include <QString>
#include <QFileInfo>

class FileRepoState;

class FileInfo
{


public:
    FileInfo(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    bool isIdenticalTo(FileInfo *fi);
    bool seemsIdenticalTo(QFileInfo q);

    QDateTime lastModified();
    QString fingerPrint();
    QString filePath();
    QString toString();
    qint64 size();
    void rename(QString newFilePath);

private:
    QDateTime m_modifiedAt; // last modification date of file
    qint64 m_sizeInBytes;
    QString m_filePath;
    QString m_sha1;
};

#endif // FILEINFO_H
