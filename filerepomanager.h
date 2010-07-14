#ifndef FILEREPOMANAGER_H
#define FILEREPOMANAGER_H

#include "filerepo.h"

class FileRepoManager
{
public:
    FileRepoManager();
    void addFileRepo(FileRepo* fileRepo); // rather, pass params?
    void removeFileRepo(FileRepo* fileRepo);
    QList<FileRepo*> listFileRepos();
    QList<FileRepo*> listFileReposContainingFile(FileInfo fileInfo);

private:

    void readSettings();
    void writeSettings();
    QList<FileRepo*> m_fileRepos;

};

#endif // FILEREPOMANAGER_H
