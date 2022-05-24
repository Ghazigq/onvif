# 编译器
CROSS_COMPILE := 

# 编译器在编译时的参数设置
CFLAGS := -Wall -Wno-unused-function -Werror -O2 -g
# 添加头文件路径
CFLAGS += -I$(PWD) -I./onvif -I./client -I./soap

# 链接器的链接参数设置 比如库文件
LDFLAGS := -lpthread -lgsoap++

EXTRA_CFLAGS := #-DDEBUG

# 添加项目中所有用到的源文件，如.c文件，子文件夹(格式/*.c)
# 添加到库中的源文件
lib-y := 

# 添加到可执行程序中的源文件
obj-y := 
obj-y += client/onvif_client_discovery.cpp

obj-y += onvif/soapC.cpp
obj-y += onvif/soapwsddProxy.cpp
# obj-y += onvif/soapwsddService.cpp

# obj-y += soap/chrono_duration.cpp
# obj-y += soap/duration.c
# obj-y += soap/stdsoap2.cpp
# obj-y += soap/wsdlC.cpp
# obj-y += soap/dom.cpp

# build目录
BUILD_PATH = build

# 可执行程序名称
TARGET ?= test

# 库名称
LIB ?= 

#展开为.o文件 增加build目录信息
TARGET := $(BUILD_PATH)/$(TARGET)
lib-y := $(wildcard $(lib-y))
lib-y := $(patsubst %.c, $(BUILD_PATH)/%.c.o, $(lib-y))
lib-y := $(patsubst %.cpp, $(BUILD_PATH)/%.cpp.o, $(lib-y))
obj-y := $(wildcard $(obj-y))
obj-y := $(patsubst %.c, $(BUILD_PATH)/%.c.o, $(obj-y))
obj-y := $(patsubst %.cpp, $(BUILD_PATH)/%.cpp.o, $(obj-y))
dep_files := $(patsubst %.o,%.d, $(lib-y) $(obj-y))

#规则
.PHONY: clean all lib target

all : target

lib : $(lib-y)
ifneq ($(lib-y),)
	@$(CROSS_COMPILE)ar crs $(BUILD_PATH)/lib$(LIB).a $(lib-y)
	@echo $(LIB) generate succeed!
endif

$(BUILD_PATH)/%.c.o : %.c
	@mkdir -p $(dir $@)
	$(CROSS_COMPILE)gcc $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $< -MMD -MP

$(BUILD_PATH)/%.cpp.o : %.cpp
	@mkdir -p $(dir $@)
	$(CROSS_COMPILE)c++ $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $< -MMD -MP

target : $(obj-y)
ifneq ($(obj-y),)
	$(CROSS_COMPILE)g++ -o $(TARGET) $(obj-y) $(LDFLAGS)
endif

clean:
	rm -rf $(BUILD_PATH)

-include $(dep_files) #必须放到后面，暂不清楚原因
