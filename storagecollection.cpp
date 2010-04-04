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

void StorageCollection::scanCollection()
{
    // read all the filenames into the hash
    if (!QFile::exists(m_path)){
        QDir dir;
        dir.mkdir(m_path);
        std::cout << "created storage collection at " << qPrintable(m_path) << std::endl;
    }
    QDir dir(m_path);
    dir.setFilter(QDir::Files);
    m_keys = dir.entryList();
    qDebug() << m_keys;
}

bool StorageCollection::addFile(QString key, QFile *file)
{
    // copy the qfile to path/key
    // return true if it worked
    // where
    m_path
}

void StorageCollection::hasFiles(QStringList *keys)
{
    //delete the entires that are not present

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
