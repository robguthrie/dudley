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
    m_createdAt = QDateTime::currentDateTime();
}

bool StateDiff::fromJSON(QByteArray json)
{
    bool ok;
    QJson::Parser parser;
    QVariant v = parser.parse(json, &ok);
    if (ok){
        return fromVariant(v);
    }else{
        qCritical() << "failed to read statediff from JSON";
        return false;
    }
}

bool StateDiff::fromVariant(QVariant v)
{
    QVariantMap wrapper = v.toMap();
    QVariantMap vmap = wrapper["state_diff"].toMap();
    m_name = vmap["name"].toString();
    m_createdAt = QDateTime::fromString(vmap["created_at"].toString(), Qt::ISODate);
    m_stateOpsHash = vmap["state_ops_hash"].toByteArray();
    QVariantList state_ops = vmap["state_ops"].toList();
    foreach(QVariant v, state_ops){
        StateOp op;
        op.fromVariant(v);
        m_stateOps.append(op);
    }
    return true;
}

QVariant StateDiff::toVariant()
{
    QVariantList state_ops_variant_list = stateOpsVariantList();
    m_stateOpsHash = stateOpsHash(state_ops_variant_list);
    m_name = m_createdAt.toString("yyyyMMddhhmmsszzz") +"_"+ m_stateOpsHash.left(8);

    QVariantMap vmap;
    vmap.insert("name", m_name);
    vmap.insert("created_at", m_createdAt.toString(Qt::ISODate));
    vmap.insert("state_ops_hash", m_stateOpsHash);
    vmap.insert("state_ops", state_ops_variant_list);
    QVariantMap wrapper;
    wrapper.insert("state_diff", vmap);
    return wrapper;
}

QByteArray StateDiff::toJSON()
{
    QJson::Serializer serializer;
    return serializer.serialize(toVariant());
}


bool StateDiff::isNew() const
{
    return m_createdAt.isNull();
}

int StateDiff::numChanges() const
{
    return m_stateOps.size();
}

QString StateDiff::name() const
{
    return m_name;
}

QDateTime StateDiff::createdAt() const
{
    return m_createdAt;
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
    foreach(op, m_stateOps){
        vlist << op.toVariant();
    }
    return vlist;
}

QList<StateOp>* StateDiff::stateOps()
{
    return &m_stateOps;
}

/* call this to log that a file has been added to a repo */
void StateDiff::addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    StateOp op;
    op.setAction("AddFile");
    op.setFilePath(file_path);
    op.setSize(size);
    op.setModifiedAt(modified_at);
    op.setFingerPrint(sha1);
    m_stateOps.append(op);
}

/* call this to log that a file has been modified in a repo */
/* TODO: check that the file exists in the state.. throw a critical or fatal if it does not */
void StateDiff::modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    StateOp op;
    op.setAction("ModifyFile");
    op.setFilePath(file_path);
    op.setSize(size);
    op.setModifiedAt(modified_at);
    op.setFingerPrint(sha1);
    m_stateOps.append(op);
}

void StateDiff::removeFile(QString file_path)
{
    StateOp op;
    op.setAction("RemoveFile");
    op.setFilePath(file_path);
    m_stateOps.append(op);
}

void StateDiff::renameFile(QString file_path, QString new_file_path)
{
    StateOp op;
    op.setAction("RenameFile");
    op.setFilePath(file_path);
    op.setNewFilePath(new_file_path);
    m_stateOps.append(op);
}
