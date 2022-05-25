
# onvif（一） C++源码生成

## 1.gsoap
### 官网地址：http://genivia.com/Products/gsoap/index.html
### 开源下载：https://sourceforge.net/projects/gsoap2

## 2.gsoap编译
### 1）解压：
```
        unzip gsoap_2.8.122.zip
```
### 2）编译：
```
        ./configure
        make
        make install
```

## 3.生成onvif.h
### 1) 指定gsoap源码目录
之后通过环境变量确定gsoap源码目录
```
        export GSOAP_PATH = ../../depository/gsoap-2.8/
```
### 2) 修改gsoap源码中gsoap/import/wsa5.h
注释函数SOAP_ENV__Fault的声明(源码末尾)，生成的onvif.h头文件会引用wsa.h wsa5.h这俩个头文件都有声明该函数，否则再蒋onvif.h转换源码时soapcpp2工具会报错
```
        //gsoap SOAP_ENV service method-action: Fault http://www.w3.org/2005/08/addressing/soap/fault
        // int SOAP_ENV__Fault
        // (       _QName			 faultcode,		// SOAP 1.1
        //         char			*faultstring,		// SOAP 1.1
        //         char			*faultactor,		// SOAP 1.1
        //         struct SOAP_ENV__Detail	*detail,		// SOAP 1.1
        //         struct SOAP_ENV__Code	*SOAP_ENV__Code,	// SOAP 1.2
        //         struct SOAP_ENV__Reason	*SOAP_ENV__Reason,	// SOAP 1.2
        //         char			*SOAP_ENV__Node,	// SOAP 1.2
        //         char			*SOAP_ENV__Role,	// SOAP 1.2
        //         struct SOAP_ENV__Detail	*SOAP_ENV__Detail,	// SOAP 1.2
        // 	void
        // );
```

### 3) typemap.dat
将源码中的gsoap/typemap.dat拷贝到当前工程目录下（这个文件之后会更改，所以不同项目单独复制）

增加Media2 namespace，这样生成的Media2相关函数会以tr2前缀开头（该步骤不是必要的，标准命名空间）
```
        tr2     = "http://www.onvif.org/ver20/media/wsdl"
```

### 4) 生成onvif.h
暂使用连接的方式生成，之后会将相关文件下载使用本地文件生成

wsdl2h使用：-o 指定输出文件 -c 生成纯c -s 不输出TLS代码 -t 指定typemap.dat文件 -x 不生成any/anyAttribute

```
        wsdl2h -o ./onvif/onvif.h -x -s -t ./typemap.dat http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl  http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl http://www.onvif.org/onvif/ver10/network/wsdl/remotediscovery.wsdl http://www.onvif.org/ver10/events/wsdl/event.wsdl http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl http://www.onvif.org/onvif/ver20/media/wsdl/media.wsdl
```
相关wsdl链接：
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

### 5) 生成源码

soapcpp2使用：-x 不生成xml文件 -j 生成c++源码 -L 不生成soapClientLib、soapServerLib -C 只生成客户端代码 -S 只生成服务端代码

```
        cd onvif && soapcpp2 onvif.h -j -x -I $GSOAP_PATH/gsoap/import -I $GSOAP_PATH/gsoap && cd -
```
