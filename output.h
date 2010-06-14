#ifndef OUTPUT_H
#define OUTPUT_H

#include <iostream>
#include <QString>

class Output
{
public:
    static void debug(QString token);
    static void error(QString token);
    static void info(QString token);

};

#endif // OUTPUT_H
