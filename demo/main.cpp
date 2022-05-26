#include <iostream>
#include "wsdd.nsmap"

#include "onvif_cli_discovery.h"

#define LOG_TAG "main"
#include "log.h"

#define DISCOVER_DEV_TIMEOUT 5
#define DISCOVER_DEV_MAX_NUM 20

static onvif_cli_dis_dev_t discovery_dev[DISCOVER_DEV_MAX_NUM];
static int test_discover_device(void) {
    int i                    = 0;
    int ret                  = 0;
    onvif_cli_dis_dev_t* dev = discovery_dev;

    ret = onvif_cli_dev_discovery(dev, DISCOVER_DEV_MAX_NUM, DISCOVER_DEV_TIMEOUT);
    if (ret > 0) {
        for (i = 0; i < ret; i++) {
            log_i("Device Address : %s", dev[i].endpoint);
            // log_i("Device name : %s", dev[i].info.name);
            // log_i("Device hardware : %s", dev[i].info.hardware);
            // log_i("Device location : %s", dev[i].info.location);
        }
    }

    return ret;
}

int main(void) {
    while (1) {
        test_discover_device();
        sleep(5);
    }
}