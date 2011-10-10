#include "synchronizermodel.h"

SynchronizerModel::SynchronizerModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int SynchronizerModel::rowCount(const QModelIndex &index) const
{
    return 3;
}

QVariant QAbstractItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    return QVariant("Name");
}

QVariant SynchronizerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return QString("ElvisSmchelvis");
    else
        return QVariant();
}
