#ifndef STATEDIFF_H
#define STATEDIFF_H
#include <QObject>
#include <QList>
#include "stateop.h"

class FileInfo;

class StateDiff
{

public:
    StateDiff();
    StateDiff(QByteArray json_ba);
    QString name() const;
    void setName(QString name);
    QByteArray serialize() const;
    QList<StateOp>* stateOps() const;

private:
    QString m_name;
    QList<StateOp> *m_stateOps;
};

#endif // STATEDIFF_H
