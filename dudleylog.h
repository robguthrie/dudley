#ifndef DUDLEYLOG_H
#define DUDLEYLOG_H

#include <QString>
#include <QHash>
#include "dudleyfileinfo.h"
class DudleyLog
{
private:
    QHash<QString, DudleyFileInfo*> m_files;

public:
    DudleyLog(QString logFile);

};

#endif // DUDLEYLOG_H
