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
    bool fromJSON(QByteArray json);
    bool fromVariant(QVariant v);
    StateDiff(QVariant v);

    QString name() const;
    bool isNew() const;
    int numChanges() const;
    QDateTime createdAt() const;
    QByteArray stateOpsHash() const;
    QList<StateOp>* stateOps();
    QVariant toVariant();
    QByteArray toJSON();

    void addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1);
    void removeFile(QString file_path);
    void renameFile(QString file_path, QString new_file_path);
private:
    QVariantList stateOpsVariantList() const;
    QByteArray stateOpsHash(QVariantList &vlist) const;

    QString m_name;
    QDateTime m_createdAt;
    QByteArray m_stateOpsHash;
    QList<StateOp> m_stateOps;
};

#endif // STATEDIFF_H
