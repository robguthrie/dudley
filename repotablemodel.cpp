#include "repotablemodel.h"
#include <QStringList>
#include "output.h"
#include "workingfilerepo.h"

QStringList RepoTableModel::columns = QString("Name Path Items Size").split(' ');

RepoTableModel::RepoTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
////    QList<FileRepo*> repoList;
////    std::cout << "repolist size: " << repoList.size();
//    repoStringList = QString("heloo you").split(" ");
//    std::cout << "repostringlist size: " << repoStringList.size();
}
int RepoTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return repoList.size();
}

int RepoTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.size();
}

QVariant RepoTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (FileRepo *repo = repoList.at(index.row())){
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

QVariant RepoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool RepoTableModel::insertRepo(FileRepo* repo)
{
    Output::info("start of insertRepo");
    std::cout << "repolist size: " << rowCount();

    Output::info("start of insertRepo2");
    beginInsertRows(QModelIndex(), 0, 0);
    repoList.append(repo);
    repoHash.insert(repo->name(), repo);
    endInsertRows();

    Output::info("got herereere33333");
//    Output::info("added repo: "+repo->name());
//    Output::info(QString("repoList.size(): ").append(QString::number(repoList.size())));
    return true;
}

bool RepoTableModel::hasRepo(QString name)
{
    return repoHash.contains(name);
}

FileRepo* RepoTableModel::repo(QString name)
{
     return repoHash.value(name);
}

