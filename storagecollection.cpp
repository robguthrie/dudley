#include "storagecollection.h"
#include <QString>
#include <QStringList>
#include <QDebug>
#include <iostream>

StorageCollection::StorageCollection(QString path)
{
    m_path = path;
    scanCollection();
}



bool StorageCollection::initialize()
{
    QDir dir;
    return dir.mkpath(m_path);
}

bool StorageCollection::isInitialized()
{
    QDir dir;
    return dir.exists(m_path);
}


void StorageCollection::scanCollection()
{
    QDir dir(m_path);
    dir.setFilter(QDir::Files);
    QString files = dir.entryList();
    QString key;
    foreach(key, files){
        m_keys.insert(key, 1);
    }
}

bool StorageCollection::contains(QString key){
    return m_keys.contains(key);
}

bool StorageCollection::addFile(QString key, QFile *file)
{

}
