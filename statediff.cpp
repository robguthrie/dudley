#include "statediff.h"
#include "parser.h"
#include "json_scanner.h"
#include "json_parser.hh"
#include "serializer.h"
#include "qobjecthelper.h"

#include <QVariantMap>
#include <QCryptographicHash>
#include <QDateTime>
#include "fileinfo.h"

StateDiff::StateDiff()
{
    m_stateOps = new QList<StateOp>;
    m_createdAt = QDateTime::currentDateTime();
}

StateDiff::StateDiff(QByteArray json_ba)
{
    m_stateOps = new QList<StateOp>;
    QJson::Parser parser;
    bool ok;
    QVariantMap vmap = parser.parse(json_ba, &ok).toMap();
    if (!ok) {
        qFatal("An error occurred during logfile parsing");
        exit(1);
    }

    m_createdAt = QDateTime::fromString(vmap["created_at"].toString(), Qt::ISODate);
    m_serializedAt = QDateTime::fromString(vmap["serialized_at"].toString(), Qt::ISODate);
    m_stateOpsHash = vmap["state_ops_hash"].toByteArray();
    QVariantList state_ops = vmap["state_ops"].toList();
    QVariant v;
    foreach(v, state_ops){
        StateOp op;
        op.fromVariant(v);
        m_stateOps->append(op);
    }
}

bool StateDiff::isNew() const
{
    return m_createdAt.isNull();
}

int StateDiff::numChanges() const
{
    return m_stateOps->size();
}

QString StateDiff::name() const
{
    return m_createdAt.toString(Qt::ISODate)+"_"+m_stateOpsHash;
}

QDateTime StateDiff::createdAt() const
{
    return m_createdAt;
}

QByteArray StateDiff::serialize()
{
    QVariantList state_ops = stateOpsVariantList();

    m_serializedAt = QDateTime::currentDateTime();
    m_stateOpsHash = stateOpsHash(state_ops);
    QVariantMap vmap;
    vmap.insert("created_at", m_createdAt.toString(Qt::ISODate));
    vmap.insert("serialized_at", m_serializedAt.toString(Qt::ISODate));
    vmap.insert("state_ops_hash", m_stateOpsHash);
    vmap.insert("state_ops", stateOpsVariantList());

    QJson::Serializer serializer;
    QByteArray body = serializer.serialize(vmap);
    qDebug() << body;
    return body;
}


QByteArray StateDiff::stateOpsHash(QVariantList &vlist) const
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QJson::Serializer serializer;
    hash.addData(serializer.serialize(vlist));
    return hash.result().toHex();
}

QVariantList StateDiff::stateOpsVariantList() const
{
    QVariantList vlist;
    StateOp op;
    foreach(op, *m_stateOps){
        vlist << op.toVariant();
    }
    return vlist;
}

QList<StateOp>* StateDiff::stateOps() const
{
    return m_stateOps;
}
