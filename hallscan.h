// Lightweight header for hallscan module
#ifndef HALLSCAN_H
#define HALLSCAN_H

#include "matrix.h"
#include "hallscan_config.h"

#include <stdbool.h>

// Public API for the hallscan module
// Initialize hardware and run calibration
void matrix_init_custom(void);

// Perform a single scan and update the provided matrix rows
bool matrix_scan_custom(matrix_row_t current_matrix[]);

// Trigger an explicit calibration (recomputes baselines and thresholds)
void hallscan_calibrate(void);

// Accessors for per-sensor data (keep storage private to hallscan.c)
uint16_t hallscan_get_baseline(sensor_id_t id);
uint16_t hallscan_get_threshold(sensor_id_t id);

// LED transistor control API (optional; no-op if HALLSCAN_LED_PIN not defined)
void led__transistor_set(bool on);
bool led__transistor_get(void);
void led__transistor_toggle(void);

#endif // HALLSCAN_H
