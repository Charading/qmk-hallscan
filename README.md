# QMK Analog MUX Module

A reusable QMK module for analog Hall-effect keyboards using HC4067 multiplexers. (ADG732 soon)
Essentially turns QMK into a firmware that works with Hall sensors.

## Features
- Supports any number of multiplexers HC4067
- Auto-calibration and per-key sensitivity
- QMK-compatible `matrix_scan_custom` and `matrix_init_custom`
- Reusable across projects via `git submodule`

### Installation

Clone into your QMK keyboard folder as a submodule:
```bash
cd qmk_firmware/keyboards/YOUR_KEYBOARD
git submodule add https://github.com/Charading/qmk-hallscan hallscan
```

### How to use

You need to update the `hallscan_config.h` and `hallscan_keymap.h` with your wiring.