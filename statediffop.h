#ifndef STATEDIFFOP_H
#define STATEDIFFOP_H

#include <QString>
#include <QDateTime>

class StateDiffOp
{
public:
    StateDiffOp();
    QString action;
    QString filePath;
    qint64  sizeInBytes;
    QDateTime modifiedAt;
    QString sha1;
};

#endif // STATEDIFFOP_H
