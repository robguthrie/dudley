#include "statediff.h"
#include "parser.h"
#include "json_scanner.h"
#include "json_parser.hh"
#include "serializer.h"
#include "qobjecthelper.h"

#include <QVariantMap>
StateDiff::StateDiff(QObject* parent)
    :QObject(parent)
{
}

QString StateDiff::name() const
{
    return m_name;
}

void StateDiff::appendOp(StateDiffOp* op)
{
    m_diffOps.append(op);
}

QVariantList StateDiff::diffOps() const
{
    QVariantList list;
    foreach(StateDiffOp* op, m_diffOps){
        list.append(QJson::QObjectHelper::qobject2qvariant(op));
    }

    return list;
}

void StateDiff::setName(QString name)
{
    m_name = name;
}

void StateDiff::setDiffOps(QVariantList list)
{
    foreach(QVariant v, list){
        StateDiffOp *op = new StateDiffOp();
        QJson::QObjectHelper::qvariant2qobject(v.toMap(), op);
        m_diffOps.append(op);
    }
}
