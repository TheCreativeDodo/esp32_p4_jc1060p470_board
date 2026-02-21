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

## Publish to ESP Component Registry

Run commands from this directory:

```bash
cd components/esp32_p4_jc1060p470_board
```

If `compote` is not on your `PATH`, use:

```bash
/home/dushan/.espressif/python_env/idf5.5_py3.13_env/bin/compote
```

### 1) Login (staging profile)

```bash
compote registry login --profile staging --registry-url https://components-staging.espressif.com
```

### 2) Validate package in staging (dry run)

```bash
compote component upload --profile staging --name esp32_p4_jc1060p470_board --namespace dushanabe --dry-run
```

### 3) Upload to staging

```bash
compote component upload --profile staging --name esp32_p4_jc1060p470_board --namespace dushanabe
```

### 4) Upload to production

```bash
compote component upload --name esp32_p4_jc1060p470_board --namespace dushanabe
```

### 5) Versioning reminder

Before each new upload, bump `version` in `idf_component.yml`.

### Troubleshooting

Check current profile config:

```bash
compote config list
```

Remove incorrect default namespace from staging profile:

```bash
compote config unset --profile staging --default-namespace
```

If `compote` is not found in a new shell, source ESP-IDF environment:

```bash
source /home/dushan/esp/v5.5.1/esp-idf/export.sh
```
