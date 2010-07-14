#include "filerepomanager.h"

FileRepoManager::FileRepoManager()
{
}

void FileRepoManager::addFileRepo(FileRepo* fileRepo)
{
    m_fileRepos.append(fileRepo);
}

void removeFileRepo(FileRepo* fileRepo)
{

}

QList<FileRepo*> listFileRepos();
QList<FileRepo*> listFileReposWithFile(FileInfo fileInfo);

private:

void readSettings();
void writeSettings();
QList<FileRepo*> m_fileRepos;
