# MARISOL.md — Pipeline Context for TTGO_TWatch

## Project Overview
ESP32-based smartwatch firmware using TTGO T-Watch 2020 V3. Uses PlatformIO for building.
















## Build & Run
- **Language**: c
- **Framework**: none
- **Docker image**: lotus-platformio:latest
- **Install deps**: (none)
- **Run**: (see source code)






## Testing
- **Test framework**: unity
- **Test command**: `pio test -e native`
- **Hardware mocks needed**: yes (arduino)
- **Last result**: 0/8 passed






## Pipeline History
- *2026-03-05* — Test phase: Fixed board ID from ttgo_twatch → esp32dev. GxEPD Serial errors blocked compilation.
- *2026-03-05* — Implement phase: Attempted to patch GxEPD source files directly (fragile). Correct fix is -DDISABLE_DIAGNOSTIC_OUTPUT build flag.
- *2026-03-05* — Implement recovery: Ran out of turns patching individual GxEPD .cpp files. Need build_flags approach instead.











- *2026-03-06* — Implement: Improve: AudioCapture.h, AudioCapture.cpp, AudioCapture_mock.h (+4 more)

- *2026-03-06* — Implement: Improve: WatchFace.cpp, WatchFace.h, test_watchface.c (+5 more)

- *2026-03-06* — Implement: Improve: BleServer.cpp, BleServer.h, test_ble_server.cpp (+4 more)

- *2026-03-06* — Implement: Improve: platformio.ini, unity_fixture.h, test_power_manager.c (+11 more)

- *2026-03-06* — Implement: Improve: WatchFace.h, WatchFace.cpp, WatchFaceTests.cpp (+2 more)

## Known Issues
- GxEPD library files (GxGDEH0154D67.cpp, GxDEPG0150BN.cpp) reference `Serial` without checking DISABLE_DIAGNOSTIC_OUTPUT — the build flag suppresses these
- Some TTGO_TWatch library headers expect specific board defines — always use LILYGO_WATCH_2020_V3
- The project has complex dependency chains — focus on getting ONE successful `pio run` before adding CI
















## Critical Build Notes
1. **GxEPD library Serial errors**: GxEPD e-paper display library uses `Serial.print()` for diagnostics. Fix: add `-DDISABLE_DIAGNOSTIC_OUTPUT` to `build_flags` in platformio.ini
2. **LILYGO_WATCH_2020_V3**: Must be defined — add `-DLILYGO_WATCH_2020_V3` to build_flags
3. **Library dependencies**: Run `pio pkg install` before `pio run` to fetch all lib_deps
4. **SensorLib for BMA423**: The BMA423 accelerometer uses SensorLib (NOT BMA423_Library which does not exist)
5. **S7XG LoRa module**: Uses HardwareSerial, requires proper Serial port assignment for ESP32
6. **AXP202X_Library**: Power management chip library — ensure it is in lib_deps
7. **LVGL**: If used, needs `lv_conf.h` properly configured
















## Recommended platformio.ini
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
build_flags = -DLILYGO_WATCH_2020_V3 -DDISABLE_DIAGNOSTIC_OUTPUT
lib_deps =
    lewisxhe/AXP202X_Library
    lewisxhe/SensorLib
```

