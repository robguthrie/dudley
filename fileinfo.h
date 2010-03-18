#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDateTime>
#include <QString>
#include <QFileInfo>

class FileInfo
{


public:
    FileInfo(QString filePath, QString collectionPath, QFileInfo *qfi);
    FileInfo(QString filePath, QString collectionPath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void update(QFileInfo *qfi);
    void update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1);
    void rename(QString newFilePath);
    QDateTime lastModified();
    QString fingerPrint();
    QString readFingerPrint();
    void updateFingerPrint();


    QString oldFilePath();
    QString filePath();
    qint64 size();

private:

    QDateTime m_modifiedAt; // last modification date of file
    qint64 m_sizeInBytes;
    QString m_filePath;
    QString m_collectionPath;
    QString m_oldFilePath;
    QString m_sha1;


};

#endif // FILEINFO_H
