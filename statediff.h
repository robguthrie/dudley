#ifndef STATEDIFF_H
#define STATEDIFF_H
#include <QObject>
#include <QList>
#include "stateop.h"

class FileInfo;

class StateDiff
{

public:
    StateDiff();
    StateDiff(QByteArray json_ba);
    QString name() const;
    bool isNew() const;
    int numChanges() const;
    QDateTime createdAt() const;
    QByteArray stateOpsHash() const;
    QByteArray serialize();
    QList<StateOp>* stateOps() const;

private:
    QVariantList stateOpsVariantList() const;
    QByteArray stateOpsHash(QVariantList &vlist) const;

    QDateTime m_createdAt;
    QDateTime m_serializedAt;
    QByteArray m_stateOpsHash;
    QList<StateOp> *m_stateOps;
};

#endif // STATEDIFF_H
