#include "repomodel.h"
#include <QStringList>
#include "output.h"

QStringList RepoModel::columns = QString("Name LocalPath TrackerUrl").split(' ');

RepoModel::RepoModel(QObject *parent)
    : QAbstractListModel(parent) {}

int RepoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

int RepoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.size();
}


QVariant RepoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (m_list.size() > index.row()){
        RepoModelItem rmi = m_list.at(index.row());
        if (role == Qt::DisplayRole){
            switch(index.column()){
            case 0:
                return rmi.name();
                break;
            case 1:
                return rmi.repo()->path();
                break;
            case 2:
                return rmi.synchronizer()->trackerUrl();
                break;
            }
        }
    }
    return QString("invalid row int");
}

QVariant RepoModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole)
        return QVariant();
    if ((section >= columns.size()) || (section < 0))
        return QVariant();

    if (orientation == Qt::Horizontal)
        return columns.at(section);
    else
        return QString("Row %1").arg(section);
}

void RepoModel::insert(RepoModelItem rmi)
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(rmi);
    endInsertRows();
}

void RepoModel::remove(QModelIndex i)
{
    beginRemoveRows(QModelIndex(), i.row(), i.row());
    RepoModelItem rmi = m_list.at(i.row());
    rmi.synchronizer()->deleteLater();
    m_list.removeAt(i.row());
    endRemoveRows();
}

bool RepoModel::hasRepo(QString name) const
{
    return names().contains(name);
}

Repo* RepoModel::repo(QString name) const
{
    for (int i =0; i < m_list.size(); i++){
        if (m_list.at(i).name() == name){
            return m_list.at(i).repo();
        }
    }
    return 0;
}

Repo* RepoModel::repo(QModelIndex i) const
{
    return m_list.at(i.row()).repo();
}

QStringList RepoModel::names() const
{
    QStringList names;
    foreach(RepoModelItem r, m_list){
        names << r.name();
    }
    return names;
}

QList<RepoModelItem> RepoModel::list()
{
    return m_list;
}

RepoModelItem::RepoModelItem(Synchronizer* sync, QString name) :
    m_synchronizer(sync), m_name(name)
{}

QString RepoModelItem::name() const
{
    return m_name;
}

Synchronizer* RepoModelItem::synchronizer() const
{
    return m_synchronizer;
}

Synchronizer* RepoModelItem::s10r() const
{
    return m_synchronizer;
}

Repo* RepoModelItem::repo() const
{
    return m_synchronizer->repo();
}
