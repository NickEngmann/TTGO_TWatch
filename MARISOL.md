# MARISOL.md — Pipeline Context

## Project Overview
TTGO T-Watch library for ESP32-based smartwatch with LVGL UI, sensors, and embedded firmware. Built with PlatformIO, supports native unit tests and ESP32 firmware builds.

## Build & Run
- **Language**: C++ (ESP32 + Native)
- **Framework**: PlatformIO, LVGL 7.x
- **Docker image**: platformio/platformio:latest
- **Install deps**: `pip install platformio` + `pio pkg install`
- **Run**: No single entry point — library/firmware project

## Testing
- **Test framework**: Unity (PlatformIO native tests)
- **Test command**: `pio test -e native -v`
- **Hardware mocks needed**: No (native tests use #ifdef NATIVE_BUILD stubs)
- **Known test issues**: 
  - Each test suite MUST be in its own directory (test/test_X/) to avoid PlatformIO main() linking conflicts
  - LVGL 7.x in repo has broken green member access — excluded via build_src_filter
  - `test_build_src = true` compiles ALL source in src/ — LVGL tests use separate native_lvgl env

## Pipeline History
- **2026-03-10**: `pio test -e native_lvgl -v` — 7 LVGL headless rendering tests pass (screenshots saved as PPM)
- **2026-03-09**: `pio test -e native -v` — 97 native unit tests pass (excludes LVGL rendering suite)
- **2026-03-08**: `pio run -e esp32` — ESP32 firmware builds successfully with all source files
- **2026-03-07**: `pio test -e native -v` — Full test suite: 104 tests total (14+20+48+15+7)

## Known Issues
- BLE audio stream requires NimBLE (ESP32 only) — native tests use #ifdef NATIVE_BUILD stubs
- ESP32 build requires BMA423.h from T-Watch BSP (not in PlatformIO registry) — CI warns, doesn't fail
- PlatformIO lib_deps (TFT_eSPI) downloaded on first build — cache ~/.platformio in CI
- LVGL tests must use native_lvgl environment (separate from native unit tests)

## Notes
- **NATIVE_BUILD Isolation**: Source files with #include <Arduino.h> are guarded with #ifdef NATIVE_BUILD
- **Test Directory Structure**: Each test suite in its own folder (test/test_step_counter/, test/test_statemachine/, etc.)
- **LVGL Headless Rendering**: Uses memory framebuffer + libpng PNG export (see PR #27)
- **Battery Monitor**: Follows StepCounter pattern with NATIVE_BUILD simulation and edge-triggered alerts (see PR #28)
- **Test Counts**: Step Counter (14), State Machine (20), BLE Audio (48), Battery Monitor (15), LVGL Rendering (7) = 104 total
