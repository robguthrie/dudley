#ifndef STATEDIFF_H
#define STATEDIFF_H
#include <QObject>
#include <QList>
#include <statediffop.h>

class StateDiff : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName);
    Q_PROPERTY(QVariantList diffOps READ diffOps WRITE setDiffOps);

public:
    StateDiff(QObject* parent = 0);

    QString name() const;
    QVariantList diffOps() const;
    void appendOp(StateDiffOp* op);
    void setName(QString name);
    void setDiffOps(QVariantList diffOps);

private:
    QString m_name;
    QList<StateDiffOp*> m_diffOps;

};

#endif // STATEDIFF_H
