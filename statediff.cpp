#include "statediff.h"
#include "parser.h"
#include "json_scanner.h"
#include "json_parser.hh"

StateDiff::StateDiff()
{
}

bool StateDiff::parseJson(QByteArray json_ba)
{
    QJson::Parser parser;
    bool ok;
    QVariantMap obj = parser.parse(json_ba, &ok).toMap();
    if (!ok){
        qDebug << "Oh no failed to parse json_ba in StateDiff";
    }else{
        m_name = obj["name"].toString();
        QVariantList diff_ops = obj["diff_ops"].toList();
        QList<QVariant>::const_iterator i;
        foreach(QVariant diff_op, diff_ops){
            StateDiffOp *op = new StateDiffOp();
            op->parseVariant(diff_op);
            m_diffOps.append(op);
        }
    }
    return ok;
}

QByteArray StateDiff::toJson()
{

}
