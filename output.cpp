#include "output.h"
#include <QStringList>


Output::Output(QString name, Output* parent) : QObject(parent){
    m_verbose = true;
    m_debug = true;
    m_name = name;
    m_parent = parent;
}

void Output::debug(QString token)
{
    if (m_debug) output("debug", token);
}

void Output::error(QString token)
{
    output("error", token);
}
void Output::info(QString token)
{
    output("i", token);
}

void Output::warning(QString token)
{
    output("warning", token);
}

void Output::verbose(QString token)
{
    if (m_verbose) output("v", token);
}

void Output::output(QString prefix, QString m)
{
    if (m_parent){
        m_parent->output(prefix, m_name+":"+m);
    }else{
        emit message(prefix+":"+m);
        std::cout << qPrintable(prefix) << ": " << qPrintable(m) << "\n";
        std::cout.flush();
    }
}
