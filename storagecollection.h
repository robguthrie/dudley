#ifndef STORAGECOLLECTION_H
#define STORAGECOLLECTION_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>

class StorageCollection
{
private:
    QString m_path;
    QHash<QString, int> m_keys;

public:
    StorageCollection(QString path);
    bool initialize();
    bool isInitialized();
    void scanCollection();
    bool contains(QString key);
};

#endif // STORAGECOLLECTION_H
