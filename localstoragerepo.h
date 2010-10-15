#ifndef STORAGEFILEREPO_H
#define STORAGEFILEREPO_H
#include "repo.h"
class LocalStorageRepo : public Repo
{
    Q_OBJECT
public:
    LocalStorageRepo(QObject* parent = 0);
};

#endif // STORAGEFILEREPO_H
