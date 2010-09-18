#ifndef HTTPCLIENTFILEREPO_H
#define HTTPCLIENTFILEREPO_H

#include "filerepostate.h"
#include "filerepo.h"
class HttpClientFileRepo : public FileRepo
{
public:
    HttpClientFileRepo(QObject *parent, QString path, QString name);
    QString type();
    bool canReadData() const;
    void updateState();
    bool hasFile(FileInfo fileInfo) const;
    QIODevice* getFile(FileInfo* fileInfo) const;

private:
    QString relativeFilePath(QString filePath);
    QString m_path;
    QString config_path;
};

#endif // HTTPCLIENTFILEREPO_H
