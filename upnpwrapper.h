#ifndef UPNPWRAPPER_H
#define UPNPWRAPPER_H
#include <QObject>
#include <QString>
#include <QThread>
#include <miniupnpc/miniwget.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>

class UpnpWrapper : public QThread
{
    Q_OBJECT
public:
    UpnpWrapper(QObject *parent);
    void run();
    void setPort(int port);
    void addRedirect();
    void removeRedirect();

signals:
    void portRedirected(bool success);

private:
    int m_port;
    struct UPNPUrls m_urls;
    struct IGDdatas m_data;
    char m_externalIPAddress[40];
    char m_lanaddr[64];	/* my ip address on the LAN */
};

#endif // UPNPWRAPPER_H
