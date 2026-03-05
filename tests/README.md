# T-Watch 2020 v3 Test Suite

This directory contains tests for verifying the T-Watch 2020 v3 hardware initialization and build configuration.

## Test Strategy

### 1. PlatformIO Configuration Test
Verifies that:
- PSRAM is configured for 8MB (0x500000 bytes)
- PSRAM type is set to OCTAL
- Arduino framework is enabled
- LVGL v7.7.2 is configured
- AXP202 driver is included

### 2. Source Files Test
Verifies that:
- TTGO.h exists
- AXP202 driver (axp20x.cpp and axp20x.h) exists
- Required source files are present

### 3. Hardware Initialization Test
Verifies that:
- test_hardware_init.cpp exists
- PSRAM size check is included
- I2C scan for AXP202 is included
- LVGL display configuration is verified

## Running Tests

```bash
cd /workspace/repo
python3 tests/test_build.py
```

## Hardware Requirements

- ESP32-D0WDQ6-V3 with 520KB SRAM + 8MB PSRAM
- Arduino Core 2.0.14
- T-Watch 2020 v3 hardware

## Build Verification

To build the project with PlatformIO:

```bash
platformio run -e twatch2020v3
```

## Test Output

The test_hardware_init.cpp sketch will output:
1. PSRAM size verification (should show 8388608 bytes)
2. I2C device scan (should find AXP202 at 0x34)
3. LVGL display configuration
4. Memory statistics (free heap and PSRAM)

## References

- [TTGO T-Watch Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)
- [T-Watch 2020 v3](https://lilygo.cc/en-us/products/t-watch-2020-v3)
- [LVGL v7.7.2](https://github.com/lvgl/lvgl)
