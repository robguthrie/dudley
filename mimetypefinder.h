#ifndef MIMETYPEFINDER_H
#define MIMETYPEFINDER_H

#include <QHash>
class MimeTypeFinder
{
public:
    MimeTypeFinder();
    QString lookup(QString ext);
private:
    QHash<QString, QString> m_mimes;
};

#endif // MIMETYPEFINDER_H
