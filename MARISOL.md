# MARISOL.md — Pipeline Context for TTGO_TWatch

## Project Overview

| Field | Value |
|-------|-------|
| **Name** | TTGO_TWatch (LilyGO T-Watch Library) |
| **Board** | LilyGO T-Watch 2019/2020 |
| **MCU** | ESP32 (Xtensa dual-core, PlatformIO + Arduino) |
| **Display** | 240x240 ST7789 SPI (1.54" square), LVGL 7.11 |
| **Sensors** | BMA423 accelerometer (step counter), AXP20X power mgmt |
| **Connectivity** | BLE (NimBLE), WiFi, IR, GPS (Air530) |
| **Touch** | FT6236 capacitive touch |
| **Audio** | PDM microphone, MAX98357 I2S amp |
| **Storage** | SPIFFS, SD card |

## Build & Run

### Native Tests (no ESP32 needed)
```bash
pio test -e native -v
```

### LVGL Simulator (headless rendering)
```bash
SIM_SCREENSHOT_DIR=screenshots pio test -e native_lvgl -v
# Screenshots saved as PPM in screenshots/
```

### ESP32 Firmware Build
```bash
pio run -e esp32
```

## Testing

### Test Suites (104 tests total)

| Suite | Directory | Tests | Coverage |
|-------|-----------|-------|----------|
| Step Counter | `test/test_native/` | 14 | Init, step counting, daily goals, reset, config, LVGL ring |
| State Machine | `test/test_statemachine/` | 20 | States (IDLE/RECORDING/STREAMING/ERROR/DEEP_SLEEP), button press, BLE connect/disconnect, timeouts, watchdog, callbacks |
| BLE Audio | `test/test_ble_audio/` | 48 | UUID validation, state transitions, audio chunks, callbacks, battery level, GATT server lifecycle |
| Battery Monitor | `test/test_battery/` | 15 | Voltage, percentage, alerts (edge-triggered), charging, power saving, config, USB detection |
| LVGL Simulator | `test/test_lvgl_render/` | 7 | Headless init, label/button/bar/arc rendering, PPM export, multi-screen navigation |

### PlatformIO Environments

| Environment | Purpose | test_build_src |
|-------------|---------|---------------|
| `native` | Unit tests (no LVGL) | false |
| `native_lvgl` | LVGL headless rendering | true (compiles `src/lvgl/`) |
| `esp32` | Firmware build | N/A |

### NATIVE_BUILD Isolation
Source files with `#include <Arduino.h>` are guarded with `#ifdef NATIVE_BUILD`. Each test includes its `.cpp` directly (`#include "../../src/Module.cpp"`). No arduino_stubs.h needed when including only isolated modules.

## CI/CD

GitHub Actions (`.github/workflows/ci.yml`) — 3 independent jobs:
1. **Native Unit Tests** — `pio test -e native -v`, 97 tests (step counter, state machine, BLE audio, battery)
2. **LVGL Simulator Tests** — `pio test -e native_lvgl -v`, 7 headless render tests, PPM→PNG conversion via netpbm, screenshots uploaded as artifacts
3. **ESP32 Build Check** — `pio run -e esp32`, installs NimBLE dep, warns (not fails) on missing BMA423.h from T-Watch BSP

## Pipeline History

| Date | Phase | Result |
|------|-------|--------|
| 2026-03-07 | Initial setup | StepCounter (14 tests), StateMachine, BatteryManager attempts |
| 2026-03-08 | LVGL simulator | Headless render tests (7 tests), BatteryMonitor (15 tests) |
| 2026-03-09 | Consolidation | All features merged, BLE audio fixed, 104 tests pass, CI/CD |
| 2026-03-09 | LVGL fix | Fixed blank screenshots (single lv_init, lv_disp_load_scr, explicit styling) |

## Known Issues

- `test_build_src = true` compiles ALL source in `src/` — LVGL tests must use separate `native_lvgl` env
- LVGL 7.x in repo (`src/lvgl/tests/` has broken `green` member access) — excluded via `build_src_filter = -<lvgl/tests/>`
- PlatformIO `lib_deps` (TFT_eSPI) downloaded on first build — cache `~/.platformio` in CI
- BLE audio stream requires NimBLE (ESP32 only) — native tests use `#ifdef NATIVE_BUILD` stubs
- ESP32 build requires BMA423.h from T-Watch BSP (not in PlatformIO registry) — CI warns, doesn't fail
- Each test suite MUST be in its own directory (PlatformIO compiles all files in a dir together → multiple-definition errors)

## LVGL Simulator Lessons (Critical for Pipeline)

- **lv_init() ONCE ONLY**: Calling lv_init() per test corrupts LVGL state → blank framebuffers. Init once, use `lvgl_test_new_screen()` per test
- **lv_disp_load_scr() not lv_scr_load()**: `lv_scr_load()` animates (300ms), `lv_disp_load_scr()` is immediate — no extra ticks needed
- **Explicit styling required**: LVGL 7.x default theme renders buttons/bars/arcs nearly invisible on white bg. Use teal (#009688) / gray (#B0BEC5) palette
- **PPM over PNG**: PPM export has zero dependencies. Convert to PNG in CI via netpbm `pnmtopng`
- **lv_obj_invalidate() + extra passes**: After widgets are created, invalidate the active screen and run 10 more lv_task_handler() calls to ensure full framebuffer flush
- **test_filter in [env:native]**: Exclude `test_lvgl_render` from native env to prevent linker conflicts with LVGL-dependent code

## Notes

- ESP32 T-Watch uses Xtensa (NOT RISC-V) — standard PlatformIO `espressif32` platform
- LVGL 7.x API: `lv_disp_buf_t`, `drv.buffer`, `lv_task_handler()` (NOT 8.x equivalents)
- Knob/touch not present — input is via touchscreen (FT6236) and BMA423 gestures
- SquareLine Studio NOT used — UI code is hand-written LVGL
- Step counter uses accelerometer simulation in native builds (deterministic rand)
- Battery monitoring uses AXP20X with edge-triggered alerts (fire once per threshold crossing)
- State machine: IDLE → RECORDING → STREAMING (on BLE) with watchdog and error recovery
- BLE state machine bug (fixed): `onConnection()` must transition to CONNECTED when in ADVERTISING state
