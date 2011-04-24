#include "statediff.h"
#include "parser.h"
#include "json_scanner.h"
#include "json_parser.hh"
#include "serializer.h"
#include "qobjecthelper.h"

#include <QVariantMap>
#include "fileinfo.h"

StateDiff::StateDiff()
{
    m_stateOps = new QList<StateOp>;
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
    QVariantList state_ops = vmap["state_ops"].toList();
    qDebug() << state_ops;
    QVariant v;
    foreach(v, state_ops){
        StateOp op;
        op.fromVariant(v);
        m_stateOps->append(op);
    }
}

QByteArray StateDiff::serialize() const
{
    QVariantMap vmap;
    vmap.insert("name", m_name);
    QVariantList vlist;
    StateOp op;
    foreach(op, *m_stateOps){
        vlist << op.toVariant();
    }
    vmap.insert("state_ops", vlist);
    QJson::Serializer serializer;
    return serializer.serialize(vmap);
}

QString StateDiff::name() const
{
    return m_name;
}

void StateDiff::setName(QString name)
{
    m_name = name;
}

QList<StateOp>* StateDiff::stateOps() const
{
    return m_stateOps;
}
