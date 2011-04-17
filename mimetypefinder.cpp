#include "mimetypefinder.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include "output.h"
MimeTypeFinder::MimeTypeFinder()
{
    // open the resource file
    // read each line
    // split line into extention and type
    // fill the hash
    QFile file(":/icons/mimetypes.txt");
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        QStringList tokens;
        while(!in.atEnd()){
            QString line = in.readLine().trimmed();
            if (line.length() > 0){
                tokens = line.split(" ");
                QString ext = tokens.at(0);
                QString mimetype = tokens.at(1);
                if ((ext.length() > 0) && (mimetype.length() > 0))
                    m_mimes[ext] = mimetype;
            }
        }
    }
}

QString MimeTypeFinder::lookup(QString ext)
{
    if (m_mimes.contains(ext)){
        return m_mimes[ext];
    }else{
        qDebug() << "no mimetype found for ext: " << ext;
        return QString();
    }
}
