
#include "soapwsddProxy.h"

#include "onvif_cli_discovery.h"

#include "onvif_common.h"
#include "wsdd.nsmap"

#define LOG_TAG "cli.dis"
#include "log.h"

int onvif_cli_dev_discovery(onvif_cli_dis_dev_t* dev, int max_nums, int timeout) {
    int ret     = 0;
    int count   = 0;
    char* index = NULL;
    wsddProxy discovery;
    struct wsdd__ProbeType req;
    struct __wsdd__ProbeMatches resp;
    char* p_begin     = NULL;
    char* p_next      = NULL;
    const char* delim = " ";
    char* chr         = NULL;
    char* scope       = NULL;

    // 设置超时时间
    discovery.soap->send_timeout = timeout;
    discovery.soap->recv_timeout = timeout;

    // 填充soap header
    char* wsa__MessageID   = soap_strdup(discovery.soap, soap_rand_uuid(discovery.soap, NULL));
    const char* wsa_To     = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
    const char* wsa_Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
    discovery.soap_header(wsa__MessageID, NULL, NULL, NULL, NULL, (char*)wsa_To, (char*)wsa_Action,
                          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    // 目的地址及端口
    discovery.soap_endpoint = "soap.udp://239.255.255.250:3702";

    // 搜索设备类型 tdn为命名空间前缀，对应http://www.onvif.org/ver10/network/wsdl
    req.Types = (char*)"tdn:NetworkVideoTransmitter";
    ret       = discovery.send_Probe(&req);
    LOG_CHECK(ret != SOAP_OK, return -1);

    do {
        ret = discovery.recv_ProbeMatches(resp);
        if (ret != SOAP_OK) {
            log_i("Find %d onvif devices!", count);
            break;
        }
        if (!resp.wsdd__ProbeMatches || !resp.wsdd__ProbeMatches->ProbeMatch ||
            resp.wsdd__ProbeMatches->__sizeProbeMatch == 0) {
            continue;
        }
        if (count < max_nums) {
            snprintf(dev[count].endpoint, sizeof(dev[count].endpoint), "%s",
                     resp.wsdd__ProbeMatches->ProbeMatch->XAddrs);
            //截断多个地址，可能会将ipv4、ipv6地址全部返回，一般默认第一个地址为ipv4
            if ((index = (char*)strstr(dev[count].endpoint, " http"))) {
                *index = '\0';
            }

            if (resp.wsdd__ProbeMatches->ProbeMatch->Scopes &&
                (p_begin = resp.wsdd__ProbeMatches->ProbeMatch->Scopes->__item) != NULL) {
                while ((chr = strtok_r(p_begin, (const char*)delim, &p_next)) != NULL &&
                       (scope = strstr(chr, "onvif://www.onvif.org/")) != NULL) {
                    scope += strlen("onvif://www.onvif.org/");

                    if ((scope = strstr(chr, "name/")) != NULL) {
                        scope += strlen("name/");
                        snprintf(dev[count].name, sizeof(dev[count].name), "%s", scope);
                    }

                    p_begin = p_next;
                }
            }

            count++;
        }
    } while (1);
    ret = count;

    return ret;
}
