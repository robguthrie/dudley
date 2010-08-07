#include "filerepo.h"

FileRepo::FileRepo(QObject *parent)
    :QObject(parent)
{
}

FileRepoState* FileRepo::state()
{
    return m_state;
}
