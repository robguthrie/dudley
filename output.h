#ifndef OUTPUT_H
#define OUTPUT_H

#include <iostream>
#include <QObject>
#include <QMetaEnum>
#include <QMetaObject>
#define ENUM_NAME(o,e,v) (o::staticMetaObject.enumerator(o::staticMetaObject.indexOfEnumerator(#e)).valueToKey((v)))

class Output : public QObject
{
    Q_OBJECT
public:
    Output(QString name = "", Output* parent = 0);
    void output(QString prefix, QString message);
    void debug(QString token);
    void error(QString token);
    void info(QString token);
    void warning(QString token);
    void verbose(QString token);
    bool m_verbose;
    bool m_debug;
    QString m_name;
    Output* m_parent;

signals:
    void message(QString);
};

extern Output* g_log;
#endif // OUTPUT_H
