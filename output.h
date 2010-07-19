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
    static void warning(QString token);
    static void verbose(QString token);
    const static bool show_verbose = true;
    const static bool show_debug = true;
};

#endif // OUTPUT_H
