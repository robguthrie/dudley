#include <QStringList>
#include "httpview.h"
#include "fileinfo.h"
#include "output.h"

QByteArray HttpView::browse(QString repo_name, QString path, QStringList sub_dirs, QList<FileInfo*> files)
{
    QStringList path_tokens;
    path_tokens << repo_name;
    if (path.trimmed().length() > 0){
        path_tokens << path.split("/");
    }
    // add the title (with nav breadcrumb)
    QByteArray body;
    body.append(QString("<h1>%1</h1>\n").arg(browseBreadCrumb(path_tokens)));
    // add the list of subdirs

    body.append(browseDirIndex(path_tokens, sub_dirs));
    // list the files in the directory

    body.append(browseFileIndex(repo_name, files));
    body.append(uploadForm(repo_name+"/"+path));
    return body;
}



QString HttpView::browseDirIndex(QStringList path_tokens, QStringList sub_dirs)
{
    QString str;
    foreach(QString dir, sub_dirs){
        QStringList temp_dirs = path_tokens;
        temp_dirs << dir;
        str.append(linkToBrowse(temp_dirs)+"<br />\n");
    }
    return str;
}

// need to merge in with
QString HttpView::browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos)
{
    QString table;
    foreach(FileInfo* f, fileInfos){
        QString link = linkToFile(repo_name, f);
        QString row = QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>\n").arg(link, humanSize(f->size()), f->mimeType());
        table.append(row);
    }
    return QString("<table>%1</table>").arg(table);
}

QString HttpView::linkToBrowse(QStringList tokens)
{
    QString repo_name = tokens.takeFirst();
    QString name;
    if (tokens.isEmpty()){
        name = repo_name;
    }else{
        name = tokens.last();
    }
    QString file_path = tokens.join("/");
    QString str("<a href=\"/%1/%2\">%3</a>");
    return str.arg(repo_name, file_path, name);
}


QString HttpView::browseBreadCrumb(QStringList dirs)
{
    QStringList links;
    QStringList temp_dirs;
    foreach(QString dir, dirs){
        temp_dirs.append(dir);
        links << linkToBrowse(temp_dirs);
    }
    return links.join("/");
}


QString HttpView::linkToFile(QString repo_name, FileInfo* f)
{
    QString str("<a href=\"/%1/%2\">%4</a>");
    return str.arg(repo_name, f->filePath(), f->fileName());
}

QString HttpView::uploadForm(QString path)
{
    QString form =
       "<h1>Upload a File</h1>"
       "<form method='post' action='/"+path+"' enctype='multipart/form-data'>"
       "File1: <input type='file' name='file1' /><br/>"
       "File2: <input type='file' name='file2' /><br/>"
       "<input type='submit' value='Upload File' />"
       "</form>";
    return form;
}
