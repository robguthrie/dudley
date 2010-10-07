#include "output.h"

void Output::debug(QString token)
{
    output("debug", token);
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
    if (show_verbose) output("v", token);
}

void Output::output(QString prefix, QString message)
{
    if (outputTextEdit) outputTextEdit->appendPlainText(message.append("\n"));
    std::cout << qPrintable(prefix) << ": " << qPrintable(message);
    std::cout.flush();
}
