#include "statediff.h"
#include "parser.h"
#include "json_scanner.h"
#include "json_parser.hh"
#include "serializer.h"
#include "qobjecthelper.h"

#include <QVariantMap>
#include "fileinfo.h"

StateDiff::StateDiff(QObject* parent)
    :QObject(parent)
{
}

QString StateDiff::name() const
{
    return m_name;
}

//void StateDiff::appendOp(StateDiffOp* op)
//{
//    m_diffOps.append(op);
//}

QVariantList StateDiff::diffOps() const
{
    QVariantList list;
    foreach(StateOp* op, m_diffOps){
        list << QVariant(op->toList());
    }

    return list;
}

QList<StateOp*> StateDiff::diffOpPtrs() const
{
    return m_diffOps;
}

void StateDiff::setName(QString name)
{
    m_name = name;
}

void StateDiff::setDiffOps(QVariantList list)
{
    foreach(QVariant v, list){
        StateOp *op = new StateOp();
        op->fromList(v.toList());
        m_diffOps << op;
    }
}

void StateDiff::addFile(FileInfo* fi)
{
    addFile(fi->filePath(), fi->size(), fi->lastModified(), fi->fingerPrint());
}

void StateDiff::modifyFile(FileInfo* fi)
{
    modifyFile(fi->filePath(), fi->size(), fi->lastModified(), fi->fingerPrint());
}

void StateDiff::addFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
{
    StateOp* op = new StateOp(this);
    op->addFile(filePath, sizeInBytes, modifiedAt,  sha1);
    m_diffOps << op;
}

void StateDiff::modifyFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
{
    StateOp* op = new StateOp(this);
    op->modifyFile(filePath, sizeInBytes, modifiedAt,  sha1);
    m_diffOps << op;
}

void StateDiff::removeFile(QString file_path)
{
    StateOp* op = new StateOp(m_diff);
    op->removeFile(file_path);
    m_diffOps << op;
}

void StateDiff::renameFile(QString file_path, QString new_file_path)
{
    StateOp* op = new StateOp(m_diff);
    op->renameFile(file_path, new_file_path);
    m_diffOps << op;
}
