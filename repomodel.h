#ifndef REPOLISTMODEL_H
#define REPOLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "repo.h"
#include "synchronizer.h"

/* a readonly model to display info about the locally mounted repos */
class RepoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    RepoModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void insert(Synchronizer *s);
    void remove(QModelIndex i);
    bool contains(QString name) const;
    Synchronizer* get(QString name) const;
    Synchronizer* get(QModelIndex i) const;

    QStringList names() const;
    QList<Synchronizer*> list();

private:
    QList<Synchronizer*> m_list;

};

#endif // REPOLISTMODEL_H
