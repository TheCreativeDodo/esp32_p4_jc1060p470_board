# Getting Started (ESP32-P4 JC1060P470 BSP)

This example demonstrates basic board bring-up using `esp32_p4_jc1060p470_board`:

- Display init + backlight on
- LVGL startup (`lv_demo_music()` by default)
- Optional startup speaker beep

## Build and flash

From this folder:

```bash
idf.py set-target esp32p4
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Configuration

Use `idf.py menuconfig`:

- `Getting Started Example -> Play startup speaker beep`
- `Getting Started Example -> Start LVGL music demo UI`

> Note: `lv_demo_music()` is a UI demo; actual SD/codec playback integration is a separate step.
