#ifndef REPOLISTMODEL_H
#define REPOLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "repo.h"
/* a readonly model to display info about the locally mounted repos */
class RepoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    RepoModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void insertRepo(Repo* repo);
    bool addRepo(QString type, QString path, QString name);
    void removeRepo(QModelIndex i);
    bool hasRepo(QString name);
    Repo* repo(QString name) const;
    Repo* repo(QModelIndex i) const;

    QStringList repoNames();
    QList<Repo*> repoList();
    void writeSettings(QSettings* s);
    void readSettings(QSettings* s);

private:
    static QStringList columns;
    QList<Repo*> m_repoList;

};

#endif // REPOLISTMODEL_H
