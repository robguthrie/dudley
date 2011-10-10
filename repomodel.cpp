#include "repomodel.h"
#include <QStringList>
#include "output.h"


RepoModel::RepoModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_list.clear();
}

int RepoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant RepoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole){
        Synchronizer *s = m_list.at(index.row());
        qDebug() << "returning data:" << s->name();
        return s->name();
    }else{
        return QVariant();
    }
}

void RepoModel::insert(Synchronizer *s)
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(s);
    endInsertRows();
}

void RepoModel::remove(QModelIndex i)
{
    beginRemoveRows(QModelIndex(), i.row(), i.row());
    Synchronizer* s = m_list.at(i.row());
    s->deleteLater();
    m_list.removeAt(i.row());
    endRemoveRows();
}

bool RepoModel::contains(QString name) const
{
    return names().contains(name);
}

Synchronizer* RepoModel::get(QString name) const
{
    for (int i =0; i < m_list.size(); i++){
        if (m_list.at(i)->name() == name){
            return m_list.at(i);
        }
    }
    return 0;
}

Synchronizer* RepoModel::get(QModelIndex i) const
{
    return m_list.at(i.row());
}

QStringList RepoModel::names() const
{
    QStringList names;
    foreach(Synchronizer *s, m_list){
        names << s->name();
    }
    return names;
}

QList<Synchronizer*> RepoModel::list()
{
    return m_list;
}
