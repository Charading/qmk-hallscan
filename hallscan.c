// HALLSCAN MODULE - Hall effect sensor matrix scanning implementation
// Based on shego75_breadboard approach

#include "hallscan.h"
#include "quantum.h"
#include "analog.h"
#include "wait.h"
#include "timer.h"
#include "print.h"

// ========================================
// INTERNAL STATE
// ========================================

// Key state tracking for debouncing
static bool key_pressed[MAX_KEYS];
static uint32_t key_timer[MAX_KEYS];
static uint32_t last_debug_time = 0;
// LED transistor cached state
static bool led_state = false;

// Sensor name strings for debug output
static const char *sensor_names[SENSOR_COUNT] = {
    "Esc", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "Bspc",
    "Tab", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Scln", "Ent",
    "LShft", "Z", "X", "C", "V", "B", "N", "M", "Comm", "Dot", "Up", "RShft",
    "LCtrl", "Win", "LAlt", "MO1", "TG3", "Spc1", "Spc2", "Fn", "RAlt", "Left", "Down", "Rght"
};

// ========================================
// MUX CHANNEL MAPPINGS
// ========================================
// These map physical MUX channels to logical sensor IDs
// The actual mappings are defined in hallscan_keymap.h

#include "hallscan_keymap.h"

// Per-sensor baseline and computed threshold (initialized at calibration)
uint16_t sensor_baseline[SENSOR_COUNT];
uint16_t sensor_thresholds[SENSOR_COUNT];

// ========================================
// HELPER FUNCTIONS
// ========================================

static void select_mux_channel(uint8_t channel) {
    writePin(MUX_S0_PIN, (channel & 0x01) ? 1 : 0);
    writePin(MUX_S1_PIN, (channel & 0x02) ? 1 : 0);
    writePin(MUX_S2_PIN, (channel & 0x04) ? 1 : 0);
    writePin(MUX_S3_PIN, (channel & 0x08) ? 1 : 0);
    wait_us(100);
}

// Read and average several ADC samples from a pin
static uint16_t sample_adc_for_pin(pin_t adc_pin) {
    uint32_t sum = 0;
    for (int i = 0; i < CALIBRATION_SAMPLES; ++i) {
        sum += analogReadPin(adc_pin);
        wait_us(500);
    }
    return (uint16_t)(sum / CALIBRATION_SAMPLES);
}

// Calibrate all mapped sensors: measure baseline and compute per-sensor threshold
void hallscan_calibrate(void) {
    uprintf("[HALLSCAN] Starting calibration (%d samples, %d%% threshold)\n", CALIBRATION_SAMPLES, SENSOR_THRESHOLD);

    // Throttle debug output to roughly once per second (reuse same mechanism
    // used by matrix_scan_custom)
    uint32_t now = timer_read32();
    bool debug_this_scan = (timer_elapsed32(last_debug_time) >= 1000);
    if (debug_this_scan) last_debug_time = now;

    pin_t adc_pins[4] = {MUX1_ADC_PIN, MUX2_ADC_PIN, MUX3_ADC_PIN, MUX4_ADC_PIN};
    const mux16_ref_t* mux_tables[4] = {mux1_channels, mux2_channels, mux3_channels, mux4_channels};

    // initialize arrays with safe defaults
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        sensor_baseline[i] = 0;
           // Default threshold 0 means "never pressed" until we calibrate a valid baseline
           sensor_thresholds[i] = 0;
    }

    for (uint8_t mux_idx = 0; mux_idx < 4; ++mux_idx) {
        for (uint8_t ch = 0; ch < 16; ++ch) {
            select_mux_channel(ch);
            wait_us(200);

            const mux16_ref_t *m = &mux_tables[mux_idx][ch];
            if (m->sensor == 0 || m->sensor > SENSOR_COUNT) continue;

            uint8_t sidx = (uint8_t)(m->sensor - 1);
            uint16_t sample = sample_adc_for_pin(adc_pins[mux_idx]);
                // Ignore floating channels that read very low values
                if (sample < ADC_MIN_VALID) {
                    if (debug_this_scan) {
                        uprintf("  MUX%d CH%d: sensor %d ignored (floating) sample=%u\n", mux_idx+1, ch, m->sensor, sample);
                    }
                    continue;
                }
            sensor_baseline[sidx] = sample;

            // compute threshold as percentage below baseline
            uint32_t thr = ((uint32_t)sensor_baseline[sidx] * (100 - (uint32_t)SENSOR_THRESHOLD)) / 100;
            if (thr > 0xFFFF) thr = 0xFFFF;
            sensor_thresholds[sidx] = (uint16_t)thr;

            uprintf(" S%02d baseline=%u thr=%u\n", sidx, sensor_baseline[sidx], sensor_thresholds[sidx]);
        }
    }
    uprintf("[HALLSCAN] Calibration complete\n");
}

// Accessor implementations
uint16_t hallscan_get_baseline(sensor_id_t id) {
    if (id == 0 || id > SENSOR_COUNT) return 0;
    uint8_t idx = (uint8_t)(id - 1);
    return sensor_baseline[idx];
}

uint16_t hallscan_get_threshold(sensor_id_t id) {
    if (id == 0 || id > SENSOR_COUNT) return 0xFFFF;
    uint8_t idx = (uint8_t)(id - 1);
    return sensor_thresholds[idx];
}

void matrix_init_custom(void) {
    // Setup MUX control pins
    setPinOutput(MUX_S0_PIN);
    setPinOutput(MUX_S1_PIN);
    setPinOutput(MUX_S2_PIN);
    setPinOutput(MUX_S3_PIN);
    // Setup LED transistor control pin (if defined per-board)
#ifdef HALLSCAN_LED_PIN
    setPinOutput(HALLSCAN_LED_PIN);
    writePin(HALLSCAN_LED_PIN, 0);
    led_state = false;
#endif
    
    // Initialize ADC pins
    setPinInputHigh(MUX1_ADC_PIN);
    setPinInputHigh(MUX2_ADC_PIN);
    setPinInputHigh(MUX3_ADC_PIN);
    setPinInputHigh(MUX4_ADC_PIN);
    
    // Initialize state
    for (uint8_t i = 0; i < MAX_KEYS; i++) {
        key_pressed[i] = false;
        key_timer[i] = 0;
    }
    
    uprintf("[HALLSCAN] Matrix initialized - 4 MUXes, %d sensors max\n", SENSOR_COUNT);

    // Run calibration at init to populate per-sensor thresholds
    hallscan_calibrate();
}

// LED transistor control API
void led__transistor_set(bool on) {
    // If pin not configured, no-op
#ifdef HALLSCAN_LED_PIN
    setPinOutput(HALLSCAN_LED_PIN);
    writePin(HALLSCAN_LED_PIN, on ? 1 : 0);
    led_state = on;
#endif
}

bool led__transistor_get(void) {
    return led_state;
}

void led__transistor_toggle(void) {
    led__transistor_set(!led_state);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;
    uint32_t now = timer_read32();
    
    // Clear matrix
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        current_matrix[row] = 0;
    }
    
    
    // Array of ADC pins and MUX tables
    pin_t adc_pins[4] = {MUX1_ADC_PIN, MUX2_ADC_PIN, MUX3_ADC_PIN, MUX4_ADC_PIN};
    const mux16_ref_t* mux_tables[4] = {mux1_channels, mux2_channels, mux3_channels, mux4_channels};
    
    // Scan all 4 MUXes
    for (uint8_t mux_idx = 0; mux_idx < 4; mux_idx++) {
        // Scan all 16 channels on this MUX
        for (uint8_t ch = 0; ch < 16; ch++) {
            select_mux_channel(ch);
            wait_us(100);
            
            uint16_t adc_val = analogReadPin(adc_pins[mux_idx]);
            
            // Get key mapping from the table
            const mux16_ref_t* key_mapping = &mux_tables[mux_idx][ch];
            
            // Skip unmapped or out-of-range sensors
            // - keymap uses 0 to represent "unmapped"
            // - the enum values are 1-based (1..SENSOR_COUNT)
            // Use `>` (not `>=`) because SENSOR_COUNT itself is a valid value.
            if (key_mapping->sensor == 0 || key_mapping->sensor > SENSOR_COUNT) {
                continue;
            }

            // Ignore floating channels / spurious low ADC readings
          
            sensor_id_t sensor = key_mapping->sensor;
            
            // Convert sensor ID to 0-based index (enum is 1-based: S_ESC=1, S_Q=2, etc.)
            uint8_t sensor_idx = sensor - 1;
            
            // Convert sensor index to matrix position (0-based index into 4x12 matrix)
            uint8_t matrix_row = sensor_idx / MATRIX_COLS;
            uint8_t matrix_col = sensor_idx % MATRIX_COLS;
            
            // Check if this is within our 4x12 matrix
            if (matrix_row >= MATRIX_ROWS || matrix_col >= MATRIX_COLS) continue;
            
            // Calculate key index for debounce tracking (use 0-based index)
            uint8_t key_idx = sensor_idx;
            
            // KEY LOGIC: Key is pressed when ADC value is BELOW per-sensor threshold
            // (SENSOR_THRESHOLD is interpreted as percent during calibration)
            uint16_t thr = sensor_thresholds[sensor_idx];
            bool should_press = (adc_val < thr);
            
            // Debounce check
            if (timer_elapsed32(key_timer[key_idx]) > DEBOUNCE_MS) {
                if (should_press != key_pressed[key_idx]) {
                    key_pressed[key_idx] = should_press;
                    key_timer[key_idx] = now;
                    changed = true;
                    
                    uprintf("Key %s: %s (R%d C%d) ADC=%d\n", 
                        sensor_names[sensor_idx],
                        should_press ? "PRESS" : "RELEASE", 
                        matrix_row, matrix_col, adc_val);
                }
            }
            
            // Set key in matrix if pressed
            if (key_pressed[key_idx]) {
                current_matrix[matrix_row] |= (1 << matrix_col);
            }
        }
    }
    
    return changed;
}
