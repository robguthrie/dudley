#include "mimetypefinder.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
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
            tokens = in.readLine().split(" ");
            QString ext = tokens.at(0);
            QString mimetype = tokens.at(1);
            m_mimes[ext] = mimetype;
        }
    }
}

QString MimeTypeFinder::lookup(QString ext)
{
    if (m_mimes.contains(ext)){
        return m_mimes[ext];
    }else{
        Output::debug("no mimetype found for ext: "+ext);
        return QString();
    }
}
