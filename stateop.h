#ifndef STATEOP_H
#define STATEOP_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>
#include <QVariantList>
class State;
class StateOp
{
public:
    StateOp();
    const static QString Actions;
    QString action() const;
    QString filePath() const;
    qint64 size() const;
    QString fingerPrint() const;
    QString newFilePath() const;
    QDateTime modifiedAt() const;
    QString toString() const;
    void setAction(QString action);
    void setFilePath(QString filePath);
    void setSize(qint64 size);
    void setFingerPrint(QString fingerPrint);
    void setNewFilePath(QString newFilePath);
    void setModifiedAt(QDateTime modified_at);

    QVariant toVariant() const;
    void fromVariant(QVariant v);

private:
    QString m_action;
    QString m_filePath;
    qint64 m_size;
    QString m_fingerPrint;
    QString m_newFilePath;
    QDateTime m_modifiedAt;
};
#endif //STATEOP_H
