#ifndef STATEDIFF_H
#define STATEDIFF_H
#include <QObject>
#include <QList>
#include <statediffop.h>

class StateDiff : public QObject
{
    Q_OBJECT
public:
    StateDiff();

private:
    QString name;
    QList<StateDiffOp> changes;
};

#endif // STATEDIFF_H
