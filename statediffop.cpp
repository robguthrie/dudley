#include "statediffop.h"

void StateDiffOp::parseVariant(QVariant op)
{
    QVariantMap map = op.toMap();
    setAction(map["name"].toString());
    setFilePath(map["filePath"].toString());
    setSize(map["size"].toLongLong());
    setLastModified(map["lastModified"].toDateTime());
    setFingerPrint(map["fingerPrint"].toString());
}

QString StateDiffOp::action()
{
    return m_action;
}

QString StateDiffOp::filePath()
{
    return m_filePath;
}

qint64 StateDiffOp::size()
{
    return m_size;
}

QDateTime StateDiffOp::lastModified()
{
    return m_lastModified;
}

QString StateDiffOp::fingerPrint()
{
    return m_fingerPrint;
}

void StateDiffOp::setAction(QString action)
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

