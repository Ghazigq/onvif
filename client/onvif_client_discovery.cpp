
// #include "onvif_client_discovery.h"

#include <iostream>
#include "wsdd.nsmap"
#include "soapwsddProxy.h"

int main()
{
    struct wsddProxy wsddTest;
    struct wsdd__ProbeType req;
    struct __wsdd__ProbeMatches rep;

    wsddTest.soap_endpoint = "soap.udp://239.255.255.250:3702";
    wsddTest.send_Probe(&req);
    wsddTest.recv_ProbeMatches(rep);
    for (int i = 0; i < rep.wsdd__ProbeMatches->__sizeProbeMatch; i++)
    {
        std::cout << rep.wsdd__ProbeMatches->ProbeMatch->XAddrs << std::endl;
    }
}
