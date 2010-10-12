#ifndef REPOLISTMODEL_H
#define REPOLISTMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "filerepo.h"
/* a readonly model to display info about the locally mounted repos */
class RepoTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RepoTableModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void insertRepo(FileRepo* repo);
    bool addRepo(QString type, QString path, QString name);
    void removeRepo(QModelIndex i);
    bool hasRepo(QString name);
    FileRepo* repo(QString name) const;
    FileRepo* repo(QModelIndex i) const;

    QStringList repoNames();
    QList<FileRepo*> repoList();
    void writeSettings(QSettings* s);
    void readSettings(QSettings* s);

private:
    static QStringList columns;
    QList<FileRepo*> m_repoList;

};

#endif // REPOLISTMODEL_H
