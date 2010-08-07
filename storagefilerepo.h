#ifndef STORAGEFILEREPO_H
#define STORAGEFILEREPO_H
#include "filerepo.h"
class StorageFileRepo : public FileRepo
{
    Q_OBJECT
public:
    StorageFileRepo(QObject* parent = 0);
};

#endif // STORAGEFILEREPO_H
