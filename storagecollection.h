#ifndef STORAGECOLLECTION_H
#define STORAGECOLLECTION_H

#include <QString>
#include <QFile>

class StorageCollection
{
private:
    QString m_path;

public:
    StorageCollection(QString path);

    // keys are supposed to be sha1 strings
    bool hasFile(QString key);
    bool addFile(QString key, QFile *file); // (key, value);
    bool removeFile(QString key);
    QFile* getFile(QString key);

};

#endif // STORAGECOLLECTION_H
