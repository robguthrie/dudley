#include "stateop.h"
#include "state.h"
#include <QDebug>
const QString StateOp::Actions = QString("AddFile ModifyFile RemoveFile RenameFile");

StateOp::StateOp()
{
    m_action = "";
    m_filePath = "";
    m_size = -1;
    m_fingerPrint = "";
    m_modifiedAt = QDateTime();
    m_newFilePath = "";
}

QString StateOp::action() const
{
    return m_action;
}

QString StateOp::filePath() const
{
    return m_filePath;
}

qint64 StateOp::size() const
{
    return m_size;
}

QString StateOp::fingerPrint() const
{
    return m_fingerPrint;
}

QString StateOp::newFilePath() const
{
    return m_newFilePath;
}

QDateTime StateOp::modifiedAt() const
{
    return m_modifiedAt;
}

void StateOp::setAction(QString action)
{
    if (StateOp::Actions.split(' ').contains(action)){
        m_action = action;
    }else{
        qWarning() << "Unrecognised action: " << action;
    }
}

void StateOp::setFilePath(QString filePath)
{
    m_filePath = filePath;
}

void StateOp::setSize(qint64 size)
{
    m_size = size;
}

void StateOp::setFingerPrint(QString fingerPrint)
{
    m_fingerPrint = fingerPrint;
}

void StateOp::setNewFilePath(QString newFilePath)
{
    m_newFilePath = newFilePath;
}

void StateOp::setModifiedAt(QDateTime modified_at)
{
    m_modifiedAt = modified_at;
}

QString StateOp::toString() const
{
   return m_filePath + QString::number(m_size) + m_modifiedAt.toString() + m_fingerPrint;
}

QVariant StateOp::toVariant() const
{
    QVariantList list;
    list << m_action;
    if ((m_action == "AddFile") || (m_action == "ModifyFile")){
        list << QVariant(m_filePath)
             << QVariant(m_size)
             << QVariant(m_modifiedAt)
             << QVariant(m_fingerPrint);
    }else if (m_action == "RemoveFile"){
        list << QVariant(m_filePath);
    }else if (m_action == "RenameFile"){
        list << QVariant(m_filePath)
             << QVariant(m_newFilePath);
    }else{
       qWarning() << "unrecognised state op action in StateOp::toList";
       return QVariant();
    }
    QVariant v(list);
    return v;
}

void StateOp::fromVariant(QVariant v)
{
    QVariantList list = v.toList();
    if (list.length() > 1){
        m_action = list.takeFirst().toString();
        /* assume well formatted from here because im lazy */
        if (m_action == "AddFile" || m_action == "ModifyFile"){
            m_filePath = list.takeFirst().toString();
            m_size = list.takeFirst().toLongLong();
            m_modifiedAt = list.takeFirst().toDateTime();
            m_fingerPrint = list.takeFirst().toString();
        }else if(m_action == "RemoveFile"){
            m_filePath = list.takeFirst().toString();
        }else if(m_action == "RenameFile"){
            m_filePath = list.takeFirst().toString();
            m_newFilePath = list.takeFirst().toString();
        }else{
            qDebug() << "unrecognised state op action in StateOp::fromList";
        }
    }else{
        qWarning() << "invalid qvariant list for stateop";
    }
}
