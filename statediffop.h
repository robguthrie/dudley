#ifndef STATEDIFFOP_H
#define STATEDIFFOP_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>
#include <QVariantList>
class State;
class StateDiffOp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString action READ action WRITE setAction);
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath);
    Q_PROPERTY(qint64 size READ size WRITE setSize);
    Q_PROPERTY(QString fingerPrint READ fingerPrint WRITE setFingerPrint);
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified);
    Q_PROPERTY(QString newFilePath READ newFilePath WRITE setNewFilePath);
public:
    const static QString Actions;

    StateDiffOp(QObject* parent = 0);
    void preformUpon(State* state) const;
    QString action() const;
    QString filePath() const;
    qint64 size() const;
    QDateTime lastModified() const;
    QString fingerPrint() const;
    QString newFilePath() const;

    void setAction(QString action);
    void setFilePath(QString filePath);
    void setSize(qint64 size);
    void setLastModified(QDateTime lastModified);
    void setFingerPrint(QString fingerPrint);
    void setNewFilePath(QString newFilePath);
    QVariantList toList() const;
    void fromList(QVariantList list);
private:
    QString m_action;
    QString m_filePath;
    qint64 m_size;
    QDateTime m_lastModified;
    QString m_fingerPrint;
    QString m_newFilePath;
};

#endif //STATEDIFFOP_H
