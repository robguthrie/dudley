#include "statediffop.h"
#include "state.h"

#include <QDebug>
const QString StateDiffOp::Actions = QString("AddFile ModifyFile RemoveFile RenameFile");

StateDiffOp::StateDiffOp(QObject* parent)
    :QObject(parent)
{
    m_action = "";
    m_filePath = "";
    m_size = -1;
    m_fingerPrint = "";
    m_lastModified = QDateTime();
    m_newFilePath = "";
}

QString StateDiffOp::action() const
{
    return m_action;
}

QString StateDiffOp::filePath() const
{
    return m_filePath;
}

qint64 StateDiffOp::size() const
{
    return m_size;
}

QDateTime StateDiffOp::lastModified() const
{
    return m_lastModified;
}

QString StateDiffOp::fingerPrint() const
{
    return m_fingerPrint;
}

QString StateDiffOp::newFilePath() const
{
    return m_newFilePath;
}

void StateDiffOp::setAction(QString action)
{
    if (!Actions.contains(action)){
        qDebug() << "invalid action: " << action;
    }
    m_action = action;
}

void StateDiffOp::setFilePath(QString filePath)
{
    m_filePath = filePath;
}

void StateDiffOp::setSize(qint64 size)
{
    m_size = size;
}

void StateDiffOp::setLastModified(QDateTime lastModified)
{
    m_lastModified = lastModified;
}

void StateDiffOp::setFingerPrint(QString fingerPrint)
{
    m_fingerPrint = fingerPrint;
}

void StateDiffOp::setNewFilePath(QString newFilePath)
{
    m_newFilePath = newFilePath;
}

QVariantList StateDiffOp::toList() const
{
    QVariantList list;
    list << QVariant(m_action)
         << QVariant(m_filePath)
         << QVariant(m_size)
         << QVariant(m_fingerPrint);
         << QVariant(m_newFilePath);
    return list;
}

void StateDiffOp::fromList(QVariantList list)
{
    m_action = list.takeFirst().toString();
    m_filePath = list.takeFirst().toString();
    m_size = list.takeFirst().toLongLong();
    m_fingerPrint = list.takeFirst().toString();
    m_newFilePath = list.takeFirst().toString();
}

void StateDiffOp::preformUpon(State *state) const
{
    if (m_action == "AddFile"){
        state->addFile(m_filePath, m_size, m_lastModified, m_fingerPrint);
    }else if (m_action == "ModifyFile"){
        state->modifyFile(m_filePath, m_size, m_lastModified, m_fingerPrint);
    }else if (m_action == "RemoveFile"){
        state->removeFile(m_filePath);
    }else if (m_action == "RenameFile"){
        state->renameFile(m_filePath, m_newFilePath);
    }else{
        qFatal() << "unknown action:" << m_action;
    }
}
