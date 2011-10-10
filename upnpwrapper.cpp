#include "upnpwrapper.h"
#include "upnperrors.h"

UpnpWrapper::UpnpWrapper(QObject* parent) :
    QThread(parent)
{
    m_port = 6789;
}

void UpnpWrapper::run()
{
    addRedirect();
}

void UpnpWrapper::setPort(int port)
{
    m_port = port;
}

void UpnpWrapper::addRedirect()
{
    struct UPNPDev * devlist;
    struct UPNPDev * dev;
    int error = 0;
    int ipv6 = 0;
    const char * rootdescurl = 0;
    const char * multicastif = 0;
    const char * minissdpdpath = 0;
    const char * iaddr = m_lanaddr;
    char iport[10];
    char eport[10];
    const char * proto = "TCP";
    const char * leaseDuration = "0";
    char intClient[40];
    char intPort[6];
    char duration[16];
    int r = 0;

    // convert port to string.
    sprintf(iport, "%d", m_port);
    sprintf(eport, "%d", m_port);

    int i  = 1;
    if( rootdescurl ||
        (devlist = upnpDiscover(2000, multicastif, minissdpdpath,
                                   0/*sameport*/, ipv6, &error))) {
        if(devlist) {
            printf("List of UPNP devices found on the network :\n");
            for(dev = devlist; dev; dev = dev->pNext) {
                printf(" desc: %s\n st: %s\n\n", dev->descURL, dev->st);
            }
        } else {
            printf("upnpDiscover() error code=%d\n", error);
        }

        if( i = UPNP_GetValidIGD(devlist, &m_urls, &m_data, m_lanaddr, sizeof(m_lanaddr))) {
            switch(i) {
            case 1:
                printf("Found valid IGD : %s\n", m_urls.controlURL);
                break;
            case 2:
                printf("Found a (not connected?) IGD : %s\n", m_urls.controlURL);
                printf("Trying to continue anyway\n");
                break;
            case 3:
                printf("UPnP device found. Is it an IGD ? : %s\n", m_urls.controlURL);
                printf("Trying to continue anyway\n");
                break;
            default:
                printf("Found device (igd ?) : %s\n", m_urls.controlURL);
                printf("Trying to continue anyway\n");
            }
            printf("Local LAN ip address : %s\n", m_lanaddr);
        }
    }

    UPNP_GetExternalIPAddress(m_urls.controlURL,
                              m_data.first.servicetype,
                              m_externalIPAddress);

    if(m_externalIPAddress[0])
        printf("ExternalIPAddress = %s\n", m_externalIPAddress);
    else
        printf("GetExternalIPAddress failed.\n");

    r = UPNP_AddPortMapping(m_urls.controlURL, m_data.first.servicetype,
                            eport, iport, iaddr, 0, proto, 0, leaseDuration);

    if(r!=UPNPCOMMAND_SUCCESS)
            printf("AddPortMapping(%s, %s, %s) failed with code %d (%s)\n",
                   eport, iport, iaddr, r, strupnperror(r));

    r = UPNP_GetSpecificPortMappingEntry(m_urls.controlURL,
                                         m_data.first.servicetype,
                                         eport, proto,
                                         intClient, intPort, NULL/*desc*/, NULL/*enabled*/, duration);
    if(r!=UPNPCOMMAND_SUCCESS)
        printf("GetSpecificPortMappingEntry() failed with code %d (%s)\n", r, strupnperror(r));

    if(intClient[0]) {
        emit portRedirected(true);
        printf("InternalIP:Port = %s:%s\n", intClient, intPort);
        printf("external %s:%s %s is redirected to internal %s:%s (duration=%s)\n",
               m_externalIPAddress, eport, proto, intClient, intPort, duration);
    }else{
        emit portRedirected(false);
    }
}

void UpnpWrapper::removeRedirect()
{
    printf("TB : upnp_rem_redir (%d)\n", m_port);
    if(m_urls.controlURL[0] == '\0')
    {
        printf("TB : the init was not done !\n");
        return;
    }
    char port_str[16];
    sprintf(port_str, "%d", m_port);
    UPNP_DeletePortMapping(m_urls.controlURL, m_data.first.servicetype, port_str, "TCP", 0);
}
