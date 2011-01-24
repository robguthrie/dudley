#ifndef STATEDIFFOP_H
#define STATEDIFFOP_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>

class StateDiffOp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Action action READ action WRITE setAction);
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath);
    Q_PROPERTY(qint64 size READ size WRITE setSize);
    Q_PROPERTY(QString fingerPrint READ fingerPrint WRITE setFingerPrint);
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified);

public:
    enum Action { AddFile, ModifyFile, RemoveFile, RenameFile };
    Q_ENUMS(Action);

    StateDiffOp(QObject* parent = 0);
    Action action() const;
    QString filePath() const;
    qint64 size() const;
    QDateTime lastModified() const;
    QString fingerPrint() const;
    void setAction(Action action);
    void setFilePath(QString filePath);
    void setSize(qint64 size);
    void setLastModified(QDateTime lastModified);
    void setFingerPrint(QString fingerPrint);

private:
    Action m_action;
    QString m_filePath;
    qint64 m_size;
    QDateTime m_lastModified;
    QString m_fingerPrint;
};

#endif //STATEDIFFOP_H
