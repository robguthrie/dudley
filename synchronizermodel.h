#ifndef SYNCHRONIZERMODEL_H
#define SYNCHRONIZERMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "synchronizer.h"

class SynchronizerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SynchronizerModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &index) const;
signals:

public slots:
private:
    QStringList m_list;
};

#endif // SYNCHRONIZERMODEL_H
