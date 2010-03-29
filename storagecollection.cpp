#include "storagecollection.h"

StorageCollection::StorageCollection(QString path)
{
    m_path = path;
    QStringlist m_keys;
}

StorageCollection::scanCollection()
{
    // read all the filenames into the hash
    QDir dir(m_path);
    dir.setFilter(QDir::Files);
    m_keys = dir.entryList();
}

bool StorageCollection::addFile(QString key, QFile *file)
{
    // copy the qfile to path/key
    // return true if it worked
}

bool StorageCollection::hasFile(QString key)
{
    // if path/key exists return true else flase
}

bool StorageCollection::removeFile(QString key)
{
    // remove the file.. return true if the file existed, false if not
}

QFile* StorageCollection::getFile(QString key)
{
    // return a QFile pointer to the file identified by key
}
