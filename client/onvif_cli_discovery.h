#ifndef onvif_cli_dis_H
#define onvif_cli_dis_H

#ifdef __cplusplus
extern "C" {
#endif

#include "onvif_common.h"

typedef struct {
    char name[ONVIF_COMMON_LEN];
    char endpoint[ONVIF_COMMON_LEN];
} onvif_cli_dis_dev_t;

int onvif_cli_dev_discovery(onvif_cli_dis_dev_t* dev, int max_nums, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* onvif_cli_dis_H */
