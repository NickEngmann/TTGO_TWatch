# T-Watch PCBA Schematics

## Overview

This directory contains the PCB schematics and layout files for all TTGO T-Watch revisions.

## Available Schematics

| Revision | File | Release Date | Status |
|----------|------|--------------|--------|
| T-Watch 2020 V1 | `T_WATCH-2020V01.pdf` | 2020-Q1 | Production |
| T-Watch 2020 V2 | `T_WATCH-2020V02.pdf` | 2020-Q3 | Production |
| T-Watch 2020 V3 | `T_WATCH-2020V03.pdf` | 2021-Q1 | Production |

## PCBA Parts Selection

For detailed parts selection analysis, supplier recommendations, and BOM specifications, see:
- **[PCBA Parts Selection Analysis (docs/pcba_parts_selection.md)](../docs/pcba_parts_selection.md)**

This document covers:
- Critical component specifications
- Supplier part numbers (DigiKey, Mouser, LCSC)
- Sourcing recommendations
- Quality control checklist
- Risk assessment and obsolescence planning

## Key Components

- **MCU:** ESP32-DOWDQ6 (Dual-core, 240 MHz, WiFi+BT)
- **PMIC:** AXP202 (Power management, battery charging, fuel gauge)
- **Accelerometer:** BMA423 (3-axis, ultra-low power, step counter)
- **Touch Controller:** FT6236 (Capacitive, 10-point touch)
- **Display Driver:** ST7789V (240x240 SPI LCD)

## Related Documentation

- [Power Consumption Analysis](../docs/power.md)
- [Pin Mapping Reference](../docs/lilypi_pinmap.md)
- [Watch 2019 Details](../docs/watch_2019.md)
- [Watch 2020 V1 Details](../docs/watch_2020_v1.md)
- [Watch 2020 V2 Details](../docs/watch_2020_v2.md)
- [Watch 2020 V3 Details](../docs/watch_2020_v3.md)

## Issues & Notes

### BMA423 Driver Integration

The BMA423 accelerometer driver requires the custom Xinyuan-LilyGO/LilyGo-HAL repository. It is not available in the standard PlatformIO library registry.

**Solution:** Add the following to `platformio.ini`:
```ini
lib_deps = 
    https://github.com/Xinyuan-LilyGO/LilyGo-HAL.git#lib/BMA423
    https://github.com/Xinyuan-LilyGO/LilyGo-HAL.git#lib/AXP20X
```

Native tests use `#ifdef NATIVE_BUILD` guards to isolate hardware dependencies.

### PlatformIO CI Warnings

The CI pipeline issues a warning (not error) for missing BMA423.h when building for ESP32. This is expected behavior and does not block the build when the custom library is properly configured.

See [MARISOL.md](../MARISOL.md) for additional CI/CD notes.

---

**Document Version:** 1.0  
**Last Updated:** 2026-03-21  
**Maintained by:** TTGO Engineering
