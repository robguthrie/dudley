#ifndef FILEREPO_H
#define FILEREPO_H

#include "filerepostate.h"

class FileRepo
{
public:

    FileRepo();

    // update the given state object to represent the current repo state
    virtual bool isReady() = 0;
    virtual void updateState() = 0;
    virtual FileRepoState* state();
    virtual bool hasFile(FileInfo fileInfo) const = 0;
//    virtual void addFile(QIODevice *sourceFile, FileInfo fileInfo) = 0;
//    virtual bool deleteFile(FileInfo fileInfo) = 0;
//    virtual bool renameFile(FileInfo fileInfo, QString newFileName) = 0;
    //    virtual QIODevice readFile(FileInfo fileInfo);
    virtual QString type() = 0;
    virtual QString path() = 0;

protected:
    FileRepoState *m_state;
};

#endif // FILEREPO_H
