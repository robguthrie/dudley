#include "filerepo.h"

FileRepo::FileRepo(QObject *parent, QString name)
    :QObject(parent), m_name(name)
{
}

FileRepoState* FileRepo::state()
{
    return m_state;
}

QString FileRepo::name()
{
    return m_name;
}

bool FileRepo::hasFileInfoByFilePath(QString file_name) const
{
    return m_state->containsFilePath(file_name);

}

bool FileRepo::hasFileInfoByFingerPrint(QString finger_print) const
{
    return m_state->containsFingerPrint(finger_print);
}


FileInfo* FileRepo::fileInfoByFingerPrint(QString finger_print) const
{
    return m_state->fileInfoByFingerPrint(finger_print);
}

FileInfo* FileRepo::fileInfoByFilePath(QString file_path) const
{
    return m_state->fileInfoByFilePath(file_path);
}
