#ifndef REPOLISTMODEL_H
#define REPOLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "repo.h"
#include "synchronizer.h"

class RepoModelItem
{
public:
    RepoModelItem(Synchronizer* sync, QString name);
    QString name() const;
    Synchronizer* s10r() const;
    Synchronizer* synchronizer() const;
    Repo* repo() const;

private:
    Synchronizer* m_synchronizer;
    QString m_name;
};

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
    void insert(RepoModelItem rmi);
    void remove(QModelIndex i);
    bool hasRepo(QString name) const;
    Repo* repo(QString name) const;
    Repo* repo(QModelIndex i) const;

    QStringList names() const;
    QList<RepoModelItem> list();
    void writeSettings(QSettings* s);
    void readSettings(QSettings* s);

private:
    static QStringList columns;
    QList<RepoModelItem> m_list;

};

#endif // REPOLISTMODEL_H
