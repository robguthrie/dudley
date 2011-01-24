#include "statediffop.h"

StateDiffOp::StateDiffOp(QObject* parent)
    :QObject(parent)
{

}

StateDiffOp::Action StateDiffOp::action() const
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

void StateDiffOp::setAction(Action action)
{
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

