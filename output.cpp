#include "output.h"


void Output::debug(QString token)
{
    if (show_debug)
    std::cout << "d: " << qPrintable(token) << std::endl;
}

void Output::error(QString token)
{
    std::cout << "error: " << qPrintable(token) << std::endl;
}
void Output::info(QString token)
{
    std::cout << "i: " << qPrintable(token) << std::endl;
}

void Output::warning(QString token)
{
    std::cout << "warning: " << qPrintable(token) << std::endl;
}

void Output::verbose(QString token)
{
    if (show_verbose)
    std::cout << "v: " << qPrintable(token) << std::endl;
}
