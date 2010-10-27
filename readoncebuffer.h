#ifndef READONCEBUFFER_H
#define READONCEBUFFER_H

#include <QIODevice>
#include <QList>
#include <QByteArray>

class ReadOnceBuffer : public QIODevice
{
    Q_OBJECT
public:
    explicit ReadOnceBuffer(QObject *parent = 0);
    bool isSequential() const;
    qint64 bytesAvailable() const;
    qint64 size() const;

protected:
    qint64 writeData ( const char * data, qint64 maxSize );
    qint64 readData  ( char * data, qint64 maxSize );

    QByteArray m_data;
};

#endif // READONCEBUFFER_H
