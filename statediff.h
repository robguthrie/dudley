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
    void parseJson(QByteArray json_ba);
private:
    QString m_name;
    QList<StateDiffOp*> m_diffOps;
};

#endif // STATEDIFF_H
