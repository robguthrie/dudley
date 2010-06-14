#ifndef FILEREPO_H
#define FILEREPO_H

#include "filerepostate.h"

class FileRepo
{
public:
//    FileRepoileRepoState* state);
//    bool containsFileByPath(QString filePath);
//    bool containsFileByFingerPrint(QString filePath);
//
    FileRepo();

    // read the state from the repo
    // passing a *lastKnownState might reduce the number of fingerprint reads required
    FileRepoState readState(FileRepoState* lastKnownState = 0);


    // manipulate the FileRepo with these functions
    // i dont think i should use fileInfo objects here....
    // we should generate our own sha1 if necessary

    //possibly addFile should return a QIODevice to write to..
    // no add file does the writing itself.. it needs to know where to read from
    // how do we know the progress of the operation?
    void addFile(QIODevice *sourceFile, QString fileName = "");
    bool deleteFileByName(QString fileName);
    bool deleteFileByFingerPrint(QString fingerPrint);
    bool renameFile(QString oldFileName, QString newFileName);


    // copy the contents of a file in this repo.. into the qiodevice
    // obiously the qiodevice must be writable
    void readFileByFingerPrint(QIODevice *destFile, QString fingerPrint);
    void readFileByFileName(QIODevice *destFile, QString fileName);




private:
};

#endif // FILEREPO_H
