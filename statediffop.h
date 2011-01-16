#ifndef STATEDIFFOP_H
#define STATEDIFFOP_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>

class StateDiffOp
{
public:
    StateDiffOp();
    void parseVariant(QVariant op);
    QString action() const;
    QString filePath() const;
    qint64 size() const;
    QDateTime lastModified() const;
    QString fingerPrint() const;
    void setAction(QString action);
    void setFilePath(QString filePath);
    void setSize(qint64 size);
    void setLastModified(QDateTime lastModified);
    void setFingerPrint(QString fingerPrint);

private:
    QString m_action;
    QString m_filePath;
    qint64 m_size;
    QDateTime m_lastModified;
    QString m_fingerPrint;
};
#endif //STATEDIFFOP_H
