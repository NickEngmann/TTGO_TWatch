# TTGO T-Watch Library

LilyGO T-Watch library for ESP32-based smartwatch with LVGL UI, sensors, and embedded firmware.

## Overview

This library provides comprehensive support for the LilyGO T-Watch 2019/2020 series smartwatches. It includes:

- **LVGL-based UI** with headless rendering support for testing
- **Step Counter** with daily goals and progress tracking
- **Battery Monitor** with voltage monitoring, charging detection, and power saving modes
- **State Machine** for managing watch states (IDLE, RECORDING, STREAMING, ERROR, DEEP_SLEEP)
- **BLE Audio** streaming support with GATT server lifecycle management
- **Sensor Integration** including BMA423 accelerometer

## Project Structure

```
TTGO_TWatch/
├── src/                    # Main library source files
│   ├── TTGO.h            # Main TTGO class header
│   ├── StepCounter.h     # Step counting functionality
│   ├── BatteryMonitor.h  # Battery monitoring
│   ├── StateMachine.h    # State machine management
│   └── lvgl/             # LVGL graphics library (v7.x)
├── test/                   # Test suites
│   ├── test_native/      # Native unit tests (no LVGL)
│   ├── test_statemachine/ # State machine tests
│   ├── test_ble_audio/   # BLE audio tests
│   ├── test_battery/     # Battery monitor tests
│   └── test_lvgl_render/ # LVGL headless rendering tests
├── docs/                   # Documentation
│   ├── power.md          # Power management guide
│   ├── pinmap.md         # Pin mapping reference
│   └── watch_2020_v3.md  # T-Watch 2020 v3 documentation
├── platformio.ini        # PlatformIO configuration
└── MARISOL.md           # Pipeline context (auto-generated)
```

## Installation

### PlatformIO Setup

This project uses PlatformIO for building and testing. Ensure you have PlatformIO installed:

```bash
pip install platformio
```

### Dependencies

The project requires the following dependencies:

- **ESP32 Arduino Framework** - Core ESP32 support
- **TFT_eSPI** - Display driver library
- **lvgl** - Light and Versatile Graphics Library (v7.x)
- **NimBLE** - BLE stack for ESP32
- **BMA423** - Accelerometer driver (from T-Watch BSP)

## Building

### ESP32 Firmware Build

Build the firmware for the ESP32 board:

```bash
pio run -e esp32
```

### Native Tests (no ESP32 needed)

Run unit tests on the host machine:

```bash
pio test -e native -v
```

### LVGL Simulator (headless rendering)

Run LVGL rendering tests with screenshot capture:

```bash
SIM_SCREENSHOT_DIR=screenshots pio test -e native_lvgl -v
```

Screenshots are saved as PPM files in the `screenshots/` directory.

## Testing

### Test Suites

The project includes **104 tests** across 5 test suites:

| Suite | Directory | Tests | Description |
|-------|-----------|-------|-------------|
| **Step Counter** | `test/test_native/` | 14 | Init, step counting, daily goals, reset, config, LVGL ring widget |
| **State Machine** | `test/test_statemachine/` | 20 | States (IDLE/RECORDING/STREAMING/ERROR/DEEP_SLEEP), button press, BLE connect/disconnect, timeouts, watchdog, callbacks |
| **BLE Audio** | `test/test_ble_audio/` | 48 | UUID validation, state transitions, audio chunks, callbacks, battery level, GATT server lifecycle |
| **Battery Monitor** | `test/test_battery/` | 15 | Voltage, percentage, alerts (edge-triggered), charging, power saving, config, USB detection |
| **LVGL Simulator** | `test/test_lvgl_render/` | 7 | Headless init, label/button/bar/arc rendering, PPM export, multi-screen navigation |

### Test Architecture

- **NATIVE_BUILD**: Source files with `#include <Arduino.h>` are guarded with `#ifdef NATIVE_BUILD`
- **Test Isolation**: Each test suite is in its own directory to avoid PlatformIO main() linking conflicts
- **Mock Architecture**: Hardware mocks are used for ESP32-specific features in native tests
- **No Arduino Stubs**: When including only isolated modules, no arduino_stubs.h is needed

### Running Tests

```bash
# Run all native tests
pio test -e native -v

# Run LVGL rendering tests
pio test -e native_lvgl -v

# Run specific test suite
pio test -e native -v -t test_step_counter
```

## PlatformIO Environments

| Environment | Purpose | test_build_src | Notes |
|-------------|---------|----------------|-------|
| `native` | Unit tests (no LVGL) | false | Excludes LVGL rendering tests |
| `native_lvgl` | LVGL headless rendering | true | Compiles `src/lvgl/`, requires PNG export |
| `esp32` | Firmware build | N/A | Builds for actual ESP32 hardware |

## Quick Reference

### Version Information
- **Library Version:** 1.4.2 (from `library.json`, `library.properties`)
- **LVGL Version:** 7.x (from `platformio.ini` build flags)
- **TFT_eSPI:** ^2.5.43 (from `library.json` lib_deps)

### File Locations
- **Source Files:** `src/*.cpp`, `src/*.h`
- **Test Files:** `test/*/test_*.cpp`
- **Documentation:** `docs/*.md`
- **Configuration:** `platformio.ini`, `library.json`, `library.properties`

### Key Headers
- `TTGO.h` — Main TTGO class for display and hardware initialization
- `StepCounter.h` — Step counting and daily goals
- `BatteryMonitor.h` — Battery voltage, percentage, charging detection
- `StateMachine.h` — Watch state management (IDLE, RECORDING, STREAMING, ERROR, DEEP_SLEEP)
- `ble_audio_stream.h` — BLE audio streaming and GATT server

## Usage Examples

### Step Counter

```cpp
#include "StepCounter.h"

StepCounter stepCounter;

void setup() {
    stepCounter.begin();
}

void loop() {
    stepCounter.update();
    Serial.printf("Steps: %lu\n", stepCounter.getSteps());
}
```

### Battery Monitor

```cpp
#include "BatteryMonitor.h"

BatteryMonitor batteryMonitor;

void setup() {
    batteryMonitor.begin();
    batteryMonitor.setAlertThreshold(20);  // Alert when below 20%
}

void loop() {
    batteryMonitor.update();
    Serial.printf("Battery: %d%% (%.2fV)\n", 
                  batteryMonitor.getPercentage(),
                  batteryMonitor.getVoltage());
}
```

### State Machine

```cpp
#include "StateMachine.h"

StateMachine stateMachine;

void setup() {
    stateMachine.begin();
}

void loop() {
    stateMachine.update();
    
    switch (stateMachine.getState()) {
        case IDLE:
            // Handle idle state
            break;
        case RECORDING:
            // Handle recording state
            break;
        case STREAMING:
            // Handle streaming state
            break;
        case ERROR:
            // Handle error state
            break;
        case DEEP_SLEEP:
            // Handle deep sleep state
            break;
    }
}
```

### LVGL UI

```cpp
#include "TTGO.h"

TTGO ttgo;

void setup() {
    ttgo.begin();
    ttgo.initDisplay();
    
    // Create UI elements
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello T-Watch!");
}
```

## Known Issues and Workarounds

### 1. LVGL 7.x Compatibility
**Issue:** The repo's `src/lvgl/tests/` has broken `green` member access.  
**Workaround:** Excluded via `build_src_filter = -<lvgl/tests/>` in `platformio.ini` (line 38).

### 2. TFT_eSPI Dependencies
**Issue:** PlatformIO `lib_deps` downloads TFT_eSPI on first build; CI may timeout.  
**Workaround:** Cache `~/.platformio` directory in CI pipeline.

### 3. BLE Audio Streaming
**Issue:** Requires NimBLE (ESP32 only); not available in native builds.  
**Workaround:** Native tests use `#ifdef NATIVE_BUILD` stubs for mock implementations.

### 4. BMA423 Driver
**Issue:** ESP32 build requires BMA423.h from T-Watch BSP (not in PlatformIO registry).  
**Workaround:** CI warns but does not fail; ensure BMA423.h is available in project tree.

### 5. Test Isolation
**Issue:** Unity framework defines `main()` in each test file; linking conflicts occur.  
**Workaround:** Each test suite MUST be in its own directory (see `test/` structure).

### 6. Native Build Isolation
**Issue:** `test_build_src = true` compiles ALL source in `src/`; conflicts with LVGL.  
**Workaround:** LVGL tests use separate `native_lvgl` environment with filtered source.

## Contributing

### Adding New Tests

1. Create a new test directory under `test/` (e.g., `test/test_new_feature/`)
2. Each test file must include its source directly: `#include "../src/NewFeature.cpp"`
3. Use Unity test framework macros: `TEST_ASSERT_EQUAL()`, `RUN_TEST()`, etc.
4. Add test environment to `platformio.ini` configuration
5. Ensure no conflicts with existing `main()` symbols

### Testing the State Machine

The State Machine supports 5 states: IDLE, RECORDING, STREAMING, ERROR, DEEP_SLEEP. See `test/test_statemachine/` for examples of:
- State transitions via button press, BLE connect/disconnect
- Configurable timeouts (idle, error recovery, watchdog)
- State change callbacks
- Edge-triggered recovery mechanisms

### Testing BLE Audio Streaming

The BLE Audio module implements a GATT server with:
- Audio service UUID: `00001800-0000-1000-8000-00805f9b34fb`
- Audio data UUID: `00002b04-0000-1000-8000-00805f9b34fb`
- Audio control UUID: `00002b05-0000-1000-8000-00805f9b34fb`
- Battery service UUID: `0000180f-0000-1000-8000-00805f9b34fb`

See `test/test_ble_audio/` for state machine and GATT server tests.

### Building for ESP32

1. Ensure all hardware dependencies are available (BMA423.h, etc.)
2. Use `pio run -e esp32` to build firmware
3. Flash using `pio run -e esp32 --target upload`

### Testing on Native Platform

1. Use `pio test -e native` for unit tests
2. Use `pio test -e native_lvgl` for LVGL rendering tests
3. Set `SIM_SCREENSHOT_DIR` for screenshot capture

## Documentation

- [Power Management Guide](docs/power.md)
- [Pin Map Reference](docs/pinmap.md)
- [T-Watch 2020 v3 Details](docs/watch_2020_v3.md)
- [T-Watch 2019 Details](docs/watch_2019.md)
- [Chinese Documentation](docs/details_cn.md)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- LilyGO for the T-Watch hardware design
- LVGL for the graphics library
- PlatformIO for the development framework
- ESP32 community for the Arduino framework

## Version History

See [CHANGELOG.md](CHANGELOG.md) for detailed version history and changes.
