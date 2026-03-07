# MARISOL.md — Pipeline Context for TTGO_TWatch

## Project Overview
Embedded C project for LilyGO T-Watch with LVGL graphics and hardware drivers, requiring PlatformIO setup for ESP32 and native Unity testing.

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
- **Last result**: 16/17 passed

## Pipeline History
- *2026-03-06* — Implement: Improve: BatteryManager.h, BatteryManager.cpp, TTGO.h (+7 more)
- *2026-03-06* — Implement: Improve: BatteryManager.h, BatteryManager.cpp, test_battery_manager.c (+4 more)
- *2026-03-06* — Implement: implemented BLE GATT server for audio streaming to phone.
- *2026-03-06* — Implement: Improve: BleServer.h, BleServer.cpp, test_bleserver.cpp
- *2026-03-06* — Implement: Improve: StateMachine.h, StateMachine.cpp, test_state_machine.cpp
- *2026-03-06* — Implement: Improve: StateMachine.cpp, StateMachine.h, test_statemachine.cpp (+1 more)

