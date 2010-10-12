#include "repotablemodel.h"
#include <QStringList>
#include "output.h"

QStringList RepoModel::columns = QString("Name Path Items Size").split(' ');

RepoModel::RepoModel(QObject *parent)
    : QAbstractListModel(parent)
{

}
//QModelIndex RepoModel::index(int row, int column, const QModelIndex & parent) const
//{
//    return createIndex(rpw, column, )
//}
int RepoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_repoList.size();
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

    if (FileRepo *repo = m_repoList.at(index.row())){
        if (role == Qt::DisplayRole){
            switch(index.column()){
            case 0:
                // name
                return repo->name();
                break;
            case 1:
                // path
                return repo->path();
                break;
            case 2:
                // items
                return QString("twenty or so");
                break;
            case 3:
                // size
                return QString("sizeable");
                break;
            default:
                return "unknown ....";
                break;
            }
        }else{
            return QVariant();
        }
    }else{
        return QString("invalid row int");
    }
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

void RepoModel::insertRepo(FileRepo* repo)
{
    beginInsertRows(QModelIndex(), m_repoList.size(), m_repoList.size());
    m_repoList.append(repo);
    endInsertRows();
}

void RepoModel::removeRepo(QModelIndex i)
{
    FileRepo* repo;
    beginRemoveRows(QModelIndex(), i.row(), i.row());
    repo = m_repoList.at(i.row());
    repo->deleteLater();
    m_repoList.removeAt(i.row());
    endRemoveRows();
}

bool RepoModel::hasRepo(QString name)
{
    return repoNames().contains(name);
}

FileRepo* RepoModel::repo(QString name) const
{
    for (int i =0; i < m_repoList.size(); i++){
        if (m_repoList.at(i)->name() == name){
//            Output::debug("RepoTableModel loading reponame: "+name);
            return m_repoList.at(i);
        }
    }
    return 0;
}

FileRepo* RepoModel::repo(QModelIndex i) const
{
    return m_repoList.at(i.row());
}
QStringList RepoModel::repoNames()
{
    QStringList names;
    foreach(FileRepo* r, m_repoList){
        names << r->name();
    }
    return names;
}

QList<FileRepo*> RepoModel::repoList()
{
    return m_repoList;
}
