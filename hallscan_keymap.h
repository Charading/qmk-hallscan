// HALLSCAN KEYMAP - MUX Channel to Sensor Mappings
// This file defines which sensor is connected to which MUX channel

#include "hallscan_config.h"

// ========================================
// MUX CHANNEL MAPPINGS
// ========================================
// Each mux16_ref_t array maps 16 MUX channels (0-15) to sensor IDs
// Use 0 for unmapped channels

// MUX 1 - Connected to MUX1_ADC_PIN
const mux16_ref_t mux1_channels[16] = {
	[0]  = { 0 },
	[1]  = { 0 },
	[2]  = { 0 },
	[3]  = { S_TAB },
	[4]  = { S_BSPC },
	[5]  = { 0 },
	[6]  = { S_U },
	[7]  = { S_I },
	[8]  = { 0 },
	[9]  = { 0 },
	[10] = { S_O },
	[11] = { S_P },
	[12] = { 0 },
	[13] = { 0 },
	[14] = { S_A },
	[15] = { S_S },
};

// MUX 2 - Connected to MUX2_ADC_PIN
const mux16_ref_t mux2_channels[16] = {
	[0]  = { 0 },
	[1]  = { 0 },
	[2]  = { 0 },
	[3]  = { 0 },
	[4]  = { 0 },
	[5]  = { S_R },
	[6]  = { S_E },
	[7]  = { S_ESC },
	[8]  = { S_Q },
	[9]  = { S_W },
	[10] = { 0 },
	[11] = { S_Y },
	[12] = { S_T },
	[13] = { 0 },
	[14] = { 0 },
	[15] = { 0 },
};

// MUX 3 - Connected to MUX3_ADC_PIN
const mux16_ref_t mux3_channels[16] = {
	[0]  = { 0 },
	[1]  = { 0 },
	[2]  = { 0 },
	[3]  = { S_D },
	[4]  = { S_R },
	[5]  = { 0 },
	[6]  = { S_F },
	[7]  = { S_G },
	[8]  = { 0 },
	[9]  = { 0 },
	[10] = { S_H },
	[11] = { S_J },
	[12] = { 0 },
	[13] = { 0 },
	[14] = { S_K },
	[15] = { S_L },
};

// MUX 4 - Connected to MUX4_ADC_PIN
const mux16_ref_t mux4_channels[16] = {
	[0]  = { 0 },
	[1]  = { 0 },
	[2]  = { 0 },
	[3]  = { 0 },
	[4]  = { 0 },
	[5]  = { S_SPC1 },
	[6]  = { S_M },
	[7]  = { S_B },
	[8]  = { S_N },
	[9]  = { S_C },
	[10] = { 0 },
	[11] = { S_V },
	[12] = { S_COMM },
	[13] = { 0 },
	[14] = { 0 },
	[15] = { 0 },
};

// -------------------------------------
// ----------     MUX 5     ------------
// -------------------------------------
// MUX5 disabled for now (external MCP3208)
/*
const mux16_ref_t mux5_channels[16] = {
	[0]  = { 0 },
	[1]  = { 0 },
	[2]  = { 0 },
	[3]  = { S_DOT },
	[4]  = { S_SCLN },
	[5]  = { 0 },
	[6]  = { S_DOWN },
	[7]  = { S_RGHT },
	[8]  = { 0 },
	[9]  = { 0 },
	[10] = { S_UP },
	[11] = { S_LEFT },
	[12] = { 0 },
	[13] = { 0 },
	[14] = { S_SPC2 },
	[15] = { S_LALT },
};
*/