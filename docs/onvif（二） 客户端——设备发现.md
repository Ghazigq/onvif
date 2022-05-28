
# onvif（二） 客户端——设备发现

## 1.onvif源码

以下为生成的onvif源码，onvif.h为临时文件，".nsmap"后缀文件为命名空间文件，"Proxy"后缀为client端源码，"Service"为server源码，"soapC"为公用文件
```
DeviceBinding.nsmap                 soapImagingBindingProxy.h      soapPullPointSubscriptionBindingProxy.cpp
ImagingBinding.nsmap                soapImagingBindingService.cpp  soapPullPointSubscriptionBindingProxy.h
Media2Binding.nsmap                 soapImagingBindingService.h    soapPullPointSubscriptionBindingService.cpp
MediaBinding.nsmap                  soapMedia2BindingProxy.cpp     soapPullPointSubscriptionBindingService.h
onvif.h                             soapMedia2BindingProxy.h       soapRemoteDiscoveryBindingProxy.cpp
PTZBinding.nsmap                    soapMedia2BindingService.cpp   soapRemoteDiscoveryBindingProxy.h
PullPointSubscriptionBinding.nsmap  soapMedia2BindingService.h     soapRemoteDiscoveryBindingService.cpp
RemoteDiscoveryBinding.nsmap        soapMediaBindingProxy.cpp      soapRemoteDiscoveryBindingService.h
soapC.cpp                           soapMediaBindingProxy.h        soapStub.h
soapDeviceBindingProxy.cpp          soapMediaBindingService.cpp    soapwsddProxy.cpp
soapDeviceBindingProxy.h            soapMediaBindingService.h      soapwsddProxy.h
soapDeviceBindingService.cpp        soapPTZBindingProxy.cpp        soapwsddService.cpp
soapDeviceBindingService.h          soapPTZBindingProxy.h          soapwsddService.h
soapH.h                             soapPTZBindingService.cpp      wsdd.nsmap
soapImagingBindingProxy.cpp         soapPTZBindingService.h
```

## 2.设备发现实现
### 1）源码添加
    本例程使用的gsoap源码文件包括：stdsoap2.cpp tdsoap2.h dom.cpp

### 2）设备发现
```
int onvif_cli_dev_discovery(int timeout) {
    int ret     = 0;
    int count   = 0;
    wsddProxy discovery;
    struct wsdd__ProbeType req;
    struct __wsdd__ProbeMatches resp;

    // 设置超时时间
    discovery.soap->send_timeout = timeout;
    discovery.soap->recv_timeout = timeout;

    ret = discovery.send_Probe(&req);
    if (ret != 0)
        return -1;

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
        printf("%s\n", resp.wsdd__ProbeMatches->ProbeMatch->XAddrs);
        count ++;
    } while (1);
    ret = count;

    return ret;
}
```

### 3）编译
    除上述文件外，onvif源码只需编译包括：soapC.cpp soapwsddProxy.cpp

----------------源码:https://github.com/Ghazigq/onvif.git-----------------
