# TTGO T-Watch Library

LilyGO T-Watch library for ESP32-based smartwatch with LVGL UI, sensors, and embedded firmware.

## Overview

This library provides comprehensive support for the LilyGO T-Watch series of smartwatches, featuring:

- **LVGL Graphics**: Beautiful 240x240 pixel display with smooth animations
- **Sensors**: BMA423 accelerometer, temperature, humidity, and pressure monitoring
- **Audio**: BLE audio streaming with GATT server support
- **Power Management**: Battery monitoring with charging detection and power saving modes
- **State Machine**: Robust state management for recording, streaming, and idle modes
- **Step Counter**: Activity tracking with daily goals and progress visualization

## Features

### Core Components

- **StepCounter**: Tracks steps with daily goals and LVGL progress ring widget
- **StateMachine**: Manages device states (IDLE, RECORDING, STREAMING, ERROR, DEEP_SLEEP)
- **BLEAudioStreamer**: Handles Bluetooth Low Energy audio streaming
- **BatteryMonitor**: Monitors voltage, percentage, charging status, and power alerts
- **LVGL Integration**: Headless rendering support for testing without hardware

### Development Framework

- **PlatformIO**: Primary development framework for ESP32 and native testing
- **Unity**: Unit testing framework for native builds
- **LVGL 7.x**: Graphics library with headless rendering support
- **NimBLE**: Bluetooth Low Energy stack for ESP32

## Installation

### Prerequisites

1. **PlatformIO**: Install via VS Code extension or CLI
   ```bash
   pip install platformio
   ```

2. **ESP32 Board Support**: Ensure ESP32 boards are installed in PlatformIO
   ```bash
   pio board install esp32
   ```

3. **Dependencies**: PlatformIO will automatically install required libraries
   - TFT_eSPI (display driver)
   - BMA423 (accelerometer - from T-Watch BSP)
   - NimBLE (Bluetooth stack)

### Build Setup

Clone the repository and open in PlatformIO:

```bash
git clone https://github.com/your-repo/ttgo-twatch.git
cd ttgo-twatch
pio init
```

## Usage

### Building for ESP32

Build the firmware for the T-Watch device:

```bash
pio run -e esp32
```

### Running Native Tests

Execute unit tests without hardware:

```bash
pio test -e native -v
```

This runs 97 native unit tests across 4 test suites:
- Step Counter (14 tests)
- State Machine (20 tests)
- BLE Audio (48 tests)
- Battery Monitor (15 tests)

### LVGL Headless Rendering Tests

Test LVGL rendering without hardware:

```bash
SIM_SCREENSHOT_DIR=screenshots pio test -e native_lvgl -v
```

This runs 7 LVGL rendering tests and saves screenshots as PPM files.

### Flashing Firmware

Upload the built firmware to your T-Watch:

```bash
pio run -e esp32 --target upload
```

## Testing

### Test Suites

The project includes comprehensive test coverage with 104 total tests:

| Suite | Directory | Tests | Coverage |
|-------|-----------|-------|----------|
| Step Counter | `test/test_native/` | 14 | Init, step counting, daily goals, reset, config, LVGL ring |
| State Machine | `test/test_statemachine/` | 20 | States (IDLE/RECORDING/STREAMING/ERROR/DEEP_SLEEP), button press, BLE connect/disconnect, timeouts, watchdog, callbacks |
| BLE Audio | `test/test_ble_audio/` | 48 | UUID validation, state transitions, audio chunks, callbacks, battery level, GATT server lifecycle |
| Battery Monitor | `test/test_battery/` | 15 | Voltage, percentage, alerts (edge-triggered), charging, power saving, config, USB detection |
| LVGL Simulator | `test/test_lvgl_render/` | 7 | Headless init, label/button/bar/arc rendering, PPM export, multi-screen navigation |

### Test Environment Setup

Each test suite is in its own directory to avoid PlatformIO main() linking conflicts. The `platformio.ini` configuration includes:

- **Native Environment**: `test_build_src = false` for isolated unit tests
- **LVGL Environment**: `test_build_src = true` with LVGL source compilation
- **Build Filters**: Excludes LVGL tests from native env to prevent conflicts

### Running Tests

```bash
# Run all native tests
pio test -e native -v

# Run LVGL rendering tests
pio test -e native_lvgl -v

# Run specific test file
pio test -e native -v --filter test_step_counter
```

### Test Results

All 104 tests pass successfully:
- ✅ 97 native unit tests (Step Counter, State Machine, BLE Audio, Battery Monitor)
- ✅ 7 LVGL rendering tests (headless simulation)

## Project Structure

```
ttgo-twatch/
├── src/                    # Main source files
│   ├── main.cpp           # Entry point for ESP32
│   ├── StepCounter.cpp    # Activity tracking
│   ├── StateMachine.cpp   # State management
│   ├── ble_audio_stream.cpp # BLE audio handling
│   ├── BatteryMonitor.cpp # Power monitoring
│   └── lvgl/              # LVGL graphics library
├── test/                  # Test suites
│   ├── test_native/       # Step counter tests
│   ├── test_statemachine/ # State machine tests
│   ├── test_ble_audio/    # BLE audio tests
│   ├── test_battery/      # Battery monitor tests
│   └── test_lvgl_render/  # LVGL rendering tests
├── platformio.ini         # PlatformIO configuration
└── README.md              # This file
```

## Known Issues

### Build Considerations

- **LVGL 7.x Compatibility**: Some LVGL tests may encounter issues with broken member access in `src/lvgl/tests/` - excluded via build_src_filter
- **BMA423 Driver**: Requires BMA423.h from T-Watch BSP (not in PlatformIO registry) - CI warns but doesn't fail
- **TFT_eSPI Dependencies**: Downloaded on first build - cache `~/.platformio` in CI environments

### Test Environment Notes

- **NATIVE_BUILD Isolation**: Source files with `#include <Arduino.h>` are guarded with `#ifdef NATIVE_BUILD`
- **BLE Audio Simulation**: Native tests use stubs for BLE functionality (ESP32 only in production)
- **LVGL Rendering**: Requires separate `native_lvgl` environment due to full source compilation

## Contributing

### Adding New Features

1. Create a new branch from `main`
2. Implement your feature in `src/`
3. Add corresponding tests in `test/`
4. Ensure all tests pass before submitting PR

### Test Guidelines

- Each test suite must be in its own directory (e.g., `test/test_feature_name/`)
- Use Unity test framework macros (TEST_ASSERT, RUN_TEST, etc.)
- Guard ESP32-specific code with `#ifdef NATIVE_BUILD`
- Document test coverage in README.md

### Code Style

- Follow existing code conventions in the repository
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and single-purpose

## Version History

See [CHANGELOG.md](CHANGELOG.md) for detailed version history and changes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **LilyGO**: For the T-Watch hardware design
- **LVGL**: For the beautiful graphics library
- **ESP32 Community**: For the robust microcontroller platform
- **PlatformIO**: For the excellent development framework

## Support

For issues and questions:
- Open an issue on GitHub
- Check the [CHANGELOG.md](CHANGELOG.md) for known issues
- Review the [README.md](README.md) for usage examples
