#include "output.h"

void Output::debug(QString token)
{
    std::cout << "dbug: " << qPrintable(token) << std::endl;
}

void Output::error(QString token)
{
    std::cout << "e! " << qPrintable(token) << std::endl;
}
void Output::info(QString token)
{
    std::cout << "i: " << qPrintable(token) << std::endl;
}
