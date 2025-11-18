#ifndef HALLSCAN_REPORTS_H
#define HALLSCAN_REPORTS_H

#include <stdint.h>

#define HALLSCAN_CMD_SET_FEATURE 0x10
#define HALLSCAN_FEATURE_RGB_ENABLED 0x01
#define HALLSCAN_FEATURE_GPIO8 0x02
#define HALLSCAN_VALUE_TOGGLE 0x02

void hallscan_handle_raw(uint8_t *data, uint8_t length);

#endif
