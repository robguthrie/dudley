#ifndef FILEREPOMANAGER_H
#define FILEREPOMANAGER_H

#include "filerepo.h"

/* this should be a dudley program manager
   should list all our fileRepos that we host
   with their formats and paths

   and list known http urls for repos that we sync against

   should this be per repo? no

   */
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
