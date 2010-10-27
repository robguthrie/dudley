#include "readoncebuffer.h"
#include "output.h"
ReadOnceBuffer::ReadOnceBuffer(QObject *parent) :
    QIODevice(parent)
{}

bool ReadOnceBuffer::isSequential() const
{
    return true;
}

qint64 ReadOnceBuffer::bytesAvailable() const
{
    return m_data.size();
}

qint64 ReadOnceBuffer::size() const
{
    return m_data.size();
}

qint64 ReadOnceBuffer::writeData ( const char * data, qint64 maxSize )
{
    g_log->debug("writing "+QString::number(maxSize)+" bytes");
    m_data.append(data, maxSize);
    return maxSize;
}

// consider keeping count of the size of the buffer.

qint64 ReadOnceBuffer::readData  ( char * data, qint64 maxSize )
{
    g_log->debug("reading "+QString::number(maxSize)+" bytes");
    qint64 n = 0;
    // read from m_pos to m_pos+maxSize
    if (m_data.size() < maxSize){
        n = m_data.size();
    }else{
        n = maxSize;
    }
    // copy n bytes into data

    strncpy(data, m_data.data(), n);
    m_data.remove(0, n);
    return n;
}
