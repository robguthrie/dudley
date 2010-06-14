#ifndef WORKINGFILEREPO_H
#define WORKINGFILEREPO_H

#include <QFile>
#include <QFileInfo>
#include "filerepostate.h"
#include "filerepo.h"

class WorkingFileRepo : public FileRepo
{
public:
    WorkingFileRepo(QString path, FileRepoState* state);
    void stageChanges();
    QStringList filesOnDisk();
    FileInfo* newFileInfo(QString filePath);
    QString readFingerPrint(QString filePath);
private:
    QString m_path;
    FileRepoState* m_state;
    void findAllFiles(QString path, QStringList *found_files);
    QString relativeFilePath(QString filePath);
};




#endif // WORKINGFILEREPO_H
















