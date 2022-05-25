# 文件修改说明

## typemap.dat

```
该文件来自于gsoap-2.8/gsoap/typemap.dat
1.使能xsd__duration = #import "custom/duration.h" | xsd__duration
2.文件末尾增加
  # 解决：PullMessages收不到事件通知
  _wsnt__NotificationMessageHolderType_Message = $ struct _tt__Message* tt__Message;

  # 解决：CreatePullPointSubscription无法订阅感兴趣的主题
  wsnt__FilterType = $ struct wsnt__TopicExpressionType* TopicExpression;

  # 解决：GetEventProperties无法解析TopicSet字段
  wstop__TopicSetType = $ _XML __mixed;
3.增加media2.wsdl命名
tr2     = "http://www.onvif.org/ver20/media/wsdl"
```

## wsa5.h

```
gsoap-2.8/gsoap/import/wsa5.h
注释函数SOAP_ENV__Fault的声明（行号277），否则使用soapcpp2会报错
```

# gsoap编译（生成gsoap工具）

```
参考https://www.genivia.com/downloads.html#noautomake
cd ./gsoap-2.8
./configure
make
cp ./gsoap/wsdl/wsdl2h ../
cp ./gsoap/src/soapcpp2 ../
make clean
cd ../
```

# onvif源码生成

```
使用前请保证处于当前目录
wsdl2h：-o 指定输出文件 -c 生成纯c -s 不输出TLS代码 -t 指定typemap.dat文件 -x 不生成any/anyAttribute
soapcpp2：-x 不生成xml文件 -L 不生成soapClientLib、soapServerLib -C 只生成客户端代码 -S 只生成服务端代码
```

## 生成onvif.h

```
wsdl2h -o ./onvif/onvif.h -x -s -t ./typemap.dat http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl  http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl http://www.onvif.org/onvif/ver10/network/wsdl/remotediscovery.wsdl http://www.onvif.org/ver10/events/wsdl/event.wsdl http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl http://www.onvif.org/onvif/ver20/media/wsdl/media.wsdl http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl

wsdl2h -o ./onvif/onvif.h -x -s -t ./typemap.dat ./wsdl/onvif.xsd ./wsdl/common.xsd ./wsdl/devicemgmt.wsdl ./wsdl/media.wsdl ./wsdl/remotediscovery.wsdl ./wsdl/event.wsdl ./wsdl/imaging.wsdl ./wsdl/media2.wsdl ./wsdl/ptz.wsdl
```

### 相关wsdl文件链接

```
http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl
http://www.onvif.org/onvif/ver10/event/wsdl/event.wsdl
http://www.onvif.org/onvif/ver10/display.wsdl
http://www.onvif.org/onvif/ver10/deviceio.wsdl
http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl
http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl
http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl
http://www.onvif.org/onvif/ver10/receiver.wsdl
http://www.onvif.org/onvif/ver10/recording.wsdl
http://www.onvif.org/onvif/ver10/search.wsdl
http://www.onvif.org/onvif/ver10/network/wsdl/remotediscovery.wsdl
http://www.onvif.org/onvif/ver10/replay.wsdl
http://www.onvif.org/onvif/ver20/analytics/wsdl/analytics.wsdl
http://www.onvif.org/onvif/ver10/analyticsdevice.wsdl
http://www.onvif.org/onvif/ver10/schema/onvif.xsd
http://www.onvif.org/ver10/actionengine.wsdl
http://www.onvif.org/onvif/ver20/media/wsdl/media.wsdl
```

## 修改onvif.h

```
1.增加#import "wsse.h"（在#import "wsa5.h"后）
2.修改struct _tev__CreatePullPointSubscriptionResponse结构体成员wsa5__EndpointReferenceType为wsa__EndpointReferenceType
```

## 生成源码

```
cd onvif && soapcpp2 onvif.h -j -x -I /home/book/ghazi/depository/gsoap-2.8/gsoap/import -I /home/book/ghazi/depository/gsoap-2.8/gsoap && cd -
```

## 修改soapStub.h

```
1.注释#define SOAP_WSA_2005 否则编译会有报错
```