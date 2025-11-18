#include "hallscan_reports.h"
#include "quantum.h"
#include "print.h"

// From adc_matrix_test.c
extern void led_toggle(void);

void hallscan_handle_raw(uint8_t *data, uint8_t length) {
    uprintf("[HID] hallscan_handle_raw: len=%d\n", length);
    if (length < 3) return;

    uint8_t *p = data;
    uint8_t len = length;

    // Handle Windows reportId=0x00
    if (len >= 4 &&
        p[0] != HALLSCAN_CMD_SET_FEATURE &&
        p[1] == HALLSCAN_CMD_SET_FEATURE) {
        p++;
        len--;
    }

    if (p[0] != HALLSCAN_CMD_SET_FEATURE) return;
    if (len < 3) return;

    uint8_t feature = p[1];
    uint8_t value   = p[2];

    uprintf("[HID] SET_FEATURE feat=%d val=%d\n", feature, value);

    switch (feature) {
        case HALLSCAN_FEATURE_RGB_ENABLED:
            if (value == HALLSCAN_VALUE_TOGGLE) {
                uprintf("[HID] led_toggle()\n");
                led_toggle();
            }
            break;

        case HALLSCAN_FEATURE_GPIO8:
            if (value == HALLSCAN_VALUE_TOGGLE) {
                uprintf("[HID] GPIO8 toggle\n");
                led_toggle();
            }
            break;

        default:
            break;
    }
}
