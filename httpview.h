#ifndef HTTPVIEW_H
#define HTTPVIEW_H
#include <QString>
#include <QList>
class FileInfo;

class HttpView
{
public:
    static QByteArray browse(QString repo_name, QString path, QStringList sub_dirs, QList<FileInfo*> files);

private:
    static QString uploadForm(QString path = "");
    static QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);
    static QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    static QString linkToBrowse(QStringList tokens);
    static QString browseBreadCrumb(QStringList dirs);
    static QString linkToFile(QString repo_name, FileInfo* f);

};

#endif // HTTPVIEW_H
