# MARISOL.md — Pipeline Context for TTGO_TWatch

## Project Overview
TTGO_TWatch is a C++ Arduino library for LilyGO T-Watch 2019/2020 smartwatches built on ESP32. The library provides LVGL-based UI, step counting, battery monitoring, state machine management, BLE audio streaming, and sensor integration (BMA423 accelerometer).

**Framework:** PlatformIO with Arduino framework  
**Target Hardware:** ESP32-based LilyGO T-Watch 2019/2020/2020 v2/v3  
**Library Version:** 1.4.2 (from library.json, line 13)  
**Key Dependencies:** ESP32 Arduino Framework, TFT_eSPI@^2.5.43, LVGL v7.x, NimBLE, BMA423

## Build/Run Instructions

### PlatformIO Setup
```bash
pip install platformio
```

### Build ESP32 Firmware
```bash
pio run -e esp32
```

### Run Native Tests (no hardware required)
```bash
pio test -e native -v
```

### Run LVGL Headless Rendering Tests
```bash
SIM_SCREENSHOT_DIR=screenshots pio test -e native_lvgl -v
```
Screenshots are saved as PPM files in the `screenshots/` directory.

## Test Suites Summary

| Suite | Directory | Tests | Description |
|-------|-----------|-------|-------------|
| **StepCounter** | `test/test_native/` | 14 | Init, step counting, daily goals, reset, config, LVGL ring widget |
| **StateMachine** | `test/test_statemachine/` | 20 | States (IDLE/RECORDING/STREAMING/ERROR/DEEP_SLEEP), button press, BLE connect/disconnect, timeouts, watchdog, callbacks |
| **BLE Audio** | `test/test_ble_audio/` | 48 | UUID validation, state transitions, audio chunks, callbacks, battery level, GATT server lifecycle |
| **Battery Monitor** | `test/test_battery/` | 15 | Voltage, percentage, edge-triggered alerts, charging, power saving, config, USB detection |
| **LVGL Renderer** | `test/test_lvgl_render/` | 7 | Headless init, label/button/bar/arc rendering, PPM export, multi-screen navigation |

**Total Tests: 104**

## CI/CD Pipeline Configuration

### PlatformIO Environments (from platformio.ini)

| Environment | Platform | Purpose | test_build_src | test_filter |
|-------------|----------|---------|----------------|-------------|
| `esp32` | espressif32 | Firmware build for actual hardware | N/A | N/A |
| `native` | native | Unit tests (no LVGL rendering) | false | test_native, test_battery, test_statemachine, test_ble_audio |
| `native_lvgl` | native | LVGL headless rendering tests | true | test_lvgl_render |

### Build Flags
- **Common:** `-DNATIVE_BUILD`, `-std=c++17`, `-DDISABLE_DIAGNOSTIC_OUTPUT`
- **native_lvgl:** `-DLV_BUILD_TEST`, `-DLV_CONF_INCLUDE_SIMPLE`, `-Isrc/lvgl/src`, `-Isrc/lvgl/..`, `-Isrc`
- **Build Source Filter (native_lvgl):** `+<lvgl/> -<lvgl/tests/>`

### lib_deps Configuration (from platformio.ini lines 17-18, 27-28, 37)
- `bodmer/TFT_eSPI@^2.5.43` — Display driver library

## Known Issues and Workarounds

1. **LVGL 7.x Compatibility** — The repo's `src/lvgl/tests/` has broken `green` member access; excluded via `build_src_filter = -<lvgl/tests/>` (platformio.ini line 38)

2. **TFT_eSPI Dependencies** — PlatformIO `lib_deps` downloads TFT_eSPI on first build; CI should cache `~/.platformio`

3. **BLE Audio Streaming** — Requires NimBLE (ESP32 only); native tests use `#ifdef NATIVE_BUILD` stubs

4. **BMA423 Driver** — ESP32 build requires BMA423.h from T-Watch BSP (not in PlatformIO registry); CI warns but does not fail

5. **Test Isolation** — Each test suite MUST be in its own directory to avoid multiple-definition errors from Unity framework `main()` symbols

6. **Native Build Isolation** — `test_build_src = true` compiles ALL source in `src/`; LVGL tests use separate `native_lvgl` environment

## LVGL Simulator Lessons

- **Memory Framebuffer:** Uses static `lv_color_t framebuffer[240*240]` for headless rendering
- **Flush Callback:** `headless_flush()` copies LVGL draw areas to framebuffer
- **Tick Handler:** `tick_val` incremented in loop with `lv_tick_inc()` and `lv_task_handler()`
- **Screen Creation:** `new_screen()` creates styled backgrounds; `lv_disp_load_scr()` immediately loads without animation
- **Screenshot Export:** PPM format exported via `save_ppm()` with RGB565 to 888 conversion
- **Test Count:** 7 tests covering init, labels, buttons, bars, arcs, export validation, multi-screen navigation

## CI Gotchas

1. **Don't run ESP32 builds in CI without hardware** — Use `native` and `native_lvgl` environments for test automation
2. **Cache PlatformIO dependencies** — TFT_eSPI download can timeout; cache `~/.platformio/lib`
3. **Environment variable for screenshots** — Set `SIM_SCREENSHOT_DIR` to control PPM output location
4. **Test file naming** — Unity test files must define `main()`; separate directories prevent linker conflicts
5. **Source file guards** — `#ifdef NATIVE_BUILD` guards must wrap all `#include <Arduino.h>` statements

## Pipeline History

- *2026-03-28* — Implemented test suite analysis: confirmed 104 total tests across 5 suites (StepCounter: 14, StateMachine: 20, BLE Audio: 48, Battery Monitor: 15, LVGL Renderer: 7)
- *2026-03-28* — Updated MARISOL.md with complete pipeline context including test counts, CI/CD configuration, and LVGL simulator lessons
- *2026-03-28* — Verified library.json (version 1.4.2) and platformio.ini configuration match README.md documentation

## Notes

- **Repository:** https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library.git (library.json lines 6-7)
- **Author:** Xinyuan Electronics <lily@lilygo.cc> (library.json line 9)
- **License:** MIT License (LICENSE file)
- **Documentation:** Additional guides in `docs/` directory (power.md, pinmap.md, watch_2019.md, watch_2020_v1.md, watch_2020_v2.md, watch_2020_v3.md)
