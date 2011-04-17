#ifndef STATEDIFF_H
#define STATEDIFF_H
#include <QObject>
#include <QList>
#include <statediffop.h>

class FileInfo;

class StateDiff : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName);
    Q_PROPERTY(QVariantList diffOps READ diffOps WRITE setDiffOps);

public:
    StateDiff(QObject* parent = 0);

    QString name() const;
    QVariantList diffOps() const;
    QList<StateOp*> diffOpPtrs() const;
    void setName(QString name);
    void setDiffOps(QVariantList diffOps);

    void addFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void modifyFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1);
    void removeFile(QString file_path);
    void renameFile(QString file_path, QString new_file_path);
private:
    QString m_name;
    QList<StateOp*> m_diffOps;
};

#endif // STATEDIFF_H
