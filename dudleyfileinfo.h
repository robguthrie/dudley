#ifndef DUDLEYFILEINFO_H
#define DUDLEYFILEINFO_H

#include <QDateTime>
#include <QString>

class DudleyFileInfo
{

private:
    QDateTime m_modifiedAt;
    qint64 m_size;
    QString m_filePath;
    QString m_sha1;

public:
    DudleyFileInfo();
};

#endif // DUDLEYFILEINFO_H
