# MARISOL.md — Pipeline Context for TTGO_TWatch

## Project Overview
Embedded C library for LilyGO T-Watch with native unit tests and LVGL headless simulation.

**Repository**: `NickEngmann/TTGO_TWatch`
**Language**: cpp
**Framework**: Arduino/PlatformIO
**Subtype**: embedded_c
**Docker Image**: `lotus-platformio:latest` — use this for all testing


## Build & Run
- **Build command**: `pio run`
- **Upload command**: `pio run -e esp32` (for ESP32 hardware)
- **Build platform**: PlatformIO (via `platformio.ini`)
- **Frameworks**: Arduino (ESP32), Native (for unit tests)


## Testing
- **Has existing tests**: yes
- **Test directory**: `test/`
- **Test framework**: Unity (embedded unit testing framework)
- **Test command**: `pio test -e native -v`
- **Test runner command**: `cd /workspace/repo && python3 -c "import re,os; 
if not os.path.exists('\''\'\'''\''platformio.ini'\''\'\'''\''): exit(); 
c=open('\''\'\'''\''platformio.ini'\''\'\'''\'').read(); 
f=re.sub(r'\''\'\'''\''(\[env:native[^\\]]*\][^\\[]*?)^(framework|board)\s*=.*$'\''\'\'''\'','\''\'\'''\''\1'\''\'\'''\'',c,flags=re.MULTILINE); 
open('\''\'\'''\''platformio.ini'\''\'\'''\'','\''\'\'''\''w'\''\'\'''\'').write(f); 
" 2>/dev/null; ENVS=$(python3 -c "import re; c=open('\''\'\'''\''platformio.ini'\''\'\'''\'').read(); 
envs=[m.group(1) for m in re.finditer(r'\''\'\'''\''\[env:(native\w*)\]'\''\'\'''\'',c)]; 
print('\''\'\'''\'' '\''\'\'''\''.join(['\''\'\'''\''-e '\''\'\'''\''+e for e in envs]) if envs else '\''\'\'''\''-e native'\''\'\'''\'') 
" 2>/dev/null || echo '\''\'\'''\''-e native'\''\'\'''\''); pio test $ENVS -v 2>&1 | tail -60 || pio run 2>&1 | tail -20`
- **Testable components**: StepCounter, BatteryMonitor, StateMachine, BLEAudioStream, LVGL_Simulator

### Test Suite Details

| Test Suite | File Path | Component Under Test | Tests |
|------------|-----------|---------------------|-------|
| test_native | test/test_native/test_step_counter.cpp | StepCounter | 14 tests |
| test_battery | test/test_battery/test_battery_monitor.cpp | BatteryMonitor | 15 tests |
| test_statemachine | test/test_statemachine/test_statemachine.cpp | StateMachine | 21 tests |
| test_ble_audio | test/test_ble_audio/test_ble_audio_stream.cpp | BLEAudioStream | 47 tests |
| test_lvgl_render | test/test_lvgl_render/test_lvgl_render.cpp | LVGL_Simulator | 7 tests |


## File Structure
**Source files (core modules):**
- `src/StepCounter.cpp/h` — Step counting with BMA423 accelerometer
- `src/BatteryMonitor.cpp/h` — Battery voltage and percentage monitoring
- `src/StateMachine.cpp/h` — Recording/streaming state machine
- `src/ble_audio_stream.cpp/h` — BLE audio streaming GATT server

**Source files (hardware support):**
- `src/TTGO.h/cpp` — Main TTGO library wrapper
- `src/LilyGoWatch.h` — Watch interface header
- `src/board/` — Hardware-specific board definitions (8 variants)
  - `twatch2019_with_touch.h`, `twatch2019_with_not_touch.h`
  - `twatch2020_v1.h`, `twatch2020_v2.h`, `twatch2020_v3.h`
  - `twatch_block.h`, `twatch_block_v1.h`
  - `lilypi_v1.h`
- `src/drive/` — Drive controller implementations
- `src/font/` — LVGL font resources
- `src/lv_conf.h` — LVGL configuration file

**Libraries (integrated):**
- `src/libraries/TFT_eSPI/` — Display driver
- `src/libraries/lv_fs_if/` — LVGL filesystem interface
- `src/libraries/lv_lib_png/` — PNG decoding for LVGL
- `src/libraries/Adafruit-GFX-Library/` — Graphics library
- `src/libraries/Adafruit-PN532/` — NFC reader
- `src/libraries/GxEPD/` — E-Paper display driver
- `src/libraries/GxEPD2/` — E-Paper display driver v2

**Test files:**
- `test/test_native/test_step_counter.cpp` — 14 StepCounter unit tests
- `test/test_battery/test_battery_monitor.cpp` — 15 BatteryMonitor tests
- `test/test_statemachine/test_statemachine.cpp` — 21 StateMachine tests
- `test/test_ble_audio/test_ble_audio_stream.cpp` — 47 BLEAudioStream tests
- `test/test_lvgl_render/test_lvgl_render.cpp` — 7 LVGL rendering tests

**Examples (Arduino IDE):**
- `examples/T_Quick/` — Quick start example
- `examples/Sensor/MAX30208/` — Heart rate sensor example
- `examples/Sensor/Fingerprint/` — Fingerprint sensor example
- `examples/U8g2_for_TFT_eSPI/` — U8g2 integration examples
- `examples/ExternTFTLibrary/` — External TFT library example
- `examples/TFT_eSPI/TFT_graphicstest_PDQ3/` — TFT display test

**Config/Build files:**
- `platformio.ini` — PlatformIO build configuration (3 environments: esp32, native, native_lvgl)
- `library.properties` — Arduino library metadata (version 1.4.2)
- `library.json` — PlatformIO library metadata


## CI Gotchas
- **Test folder structure**: Each test group MUST be in its own folder (test/test_X/) to avoid PlatformIO main() linking conflicts
- **Native build filter**: When compiling for native tests, use `test_build_src=false` for main modules and `test_build_src=true` for LVGL tests
- **PNG library**: LVGL rendering tests require libpng integration via `add_png_lib.py` in project conf
- **Random number generation**: Native builds require mocked `rand()` function to ensure deterministic test results
- **Memory framebuffer**: LVGL headless rendering requires manual flush callback to memory buffer for PPM export


## Pipeline History
- 2026-03-16 — Scout: cpp/none, scope=partial
- 2026-03-16 — Implement: Completed LVGL headless rendering implementation with memory framebuffer and libpng PNG export
- 2026-03-16 — Implement: Added BatteryMonitor following StepCounter pattern with NATIVE_BUILD simulation and edge-triggered alerts
- 2026-03-16 — Implement: Created StateMachine unit tests with full state transition coverage
- 2026-03-16 — Implement: Implemented BLEAudioStream with GATT server mock for native testing
- 2026-03-16 — Implement: Added comprehensive test suites for all modular components
- 2026-03-17 — Test: Verified all 105 unit tests pass across native and native_lvgl environments (20 StateMachine, 14 StepCounter, 48 BLEAudioStream, 15 BatteryMonitor, 7 LVGL render tests)

## Summary

**What I fixed:**
- **Removed `test/test_native/test_placeholder.c`** - This file c

## Known Issues
(none yet)


## Notes
- This file is auto-read by Qwen Code as project context (like CLAUDE.md)
- Updated by each pipeline phase with learnings, CI fixes, and gotchas
- DO NOT delete this file — it helps the pipeline avoid repeating mistakes
- PlatformIO native builds use Unity test framework with separate test folders
- LVGL 7.7.2 is embedded as a submodule with custom PNG library support
- All modules support both ESP32 hardware builds and NATIVE_BUILD simulation
- **Lesson learned**: PlatformIO native tests each test group MUST be in its own folder (test/test_X/) to avoid PlatformIO main() linking conflicts. platformio.ini needs `build_src_filter=+<lvgl/>` for LVGL, `test_build_src=true`, and `add_png_lib.py` for libpng.

## Key Source Code Snippets

### StepCounter Interface (src/StepCounter.h)
```cpp
class StepCounter {
public:
    struct Config {
        uint32_t dailyGoal = 10000;
        bool enableNotifications = true;
        bool enableAutoReset = true;
    };

    struct Status {
        uint32_t dailySteps = 0;
        uint32_t totalSteps = 0;
        uint32_t stepsSinceMidnight = 0;
        uint32_t lastResetTime = 0;
        bool goalReached = false;
        bool initialized = false;
    };

    bool init();
    void update();
    uint32_t getStepCount() const;
    void setProgressRingWidget(void* ringWidget);
    // ... more methods
};
```

### BatteryMonitor Interface (src/BatteryMonitor.h)
```cpp
class BatteryMonitor {
public:
    struct Config {
        int lowBatteryThreshold = 20;
        int criticalThreshold = 5;
        bool enableAlerts = true;
        bool enablePowerSaving = true;
        uint32_t pollIntervalMs = 5000;
    };

    struct Status {
        float voltage = 0.0f;
        int percentage = 0;
        bool isCharging = false;
        bool isUSBConnected = false;
        float temperature = 0.0f;
        // ... more fields
    };

    bool init();
    void update();
    int getPercentage() const;
    // ... more methods
};
```

### StateMachine Interface (src/StateMachine.h)
```cpp
class StateMachine {
public:
    enum State {
        IDLE,
        RECORDING,
        STREAMING,
        ERROR,
        DEEP_SLEEP
    };

    struct Config {
        uint32_t idleTimeoutMs = 30000;
        uint32_t errorRecoveryMs = 5000;
        uint32_t watchdogTimeoutMs = 60000;
    };

    bool init();
    void update(uint32_t currentTimeMs);
    State getCurrentState() const;
    // ... more methods
};
```

### BLEAudioStream Interface (src/ble_audio_stream.h)
```cpp
class BLEAudioStreamer {
public:
    enum class AudioStreamState {
        IDLE, ADVERTISING, CONNECTED, STREAMING, ERROR
    };

    enum class AudioControlCommand {
        START_STREAM, STOP_STREAM, PAUSE_STREAM, RESUME_STREAM
    };

    bool startStreaming();
    bool sendAudioChunk(const uint8_t* data, uint16_t size);
    uint8_t getBatteryLevel() const;
    // ... more methods
};
```


