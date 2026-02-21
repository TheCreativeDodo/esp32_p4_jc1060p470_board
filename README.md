# ESP32-P4 JC1060P470 BSP

Board Support Package (BSP) component for ESP32-P4 JC1060P470 display boards.

## Highlights

- MIPI-DSI LCD support (JD9165 1024x600)
- Optional ILI9881C and LT8912B paths retained for compatibility
- LVGL integration via `esp_lvgl_port`
- Touch via GT911
- Audio, storage, USB host helper APIs aligned with ESP BSP patterns

## Public headers

- `bsp/esp-bsp.h` (umbrella include)
- `bsp/esp32_p4_jc1060p470_board.h` (canonical board header)
- `bsp/esp32_p4_function_ev_board.h` (compatibility alias)
- `bsp/audio_player_helpers.h` (optional integrated audio-player helpers)

## Example

- `examples/getting_started` — minimal bring-up example intended for publishing/registry use

## Notes

This BSP includes a custom JD9165 initialization command table derived from a known-good board variant. This is required for stable panel bring-up on JC1060P470 hardware.

Audio player helper APIs can be enabled/disabled via `CONFIG_BSP_ENABLE_AUDIO_PLAYER_HELPERS`.