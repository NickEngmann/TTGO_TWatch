# TTGO T-Watch PCBA Parts Selection Guide

## Overview

This document provides a comprehensive parts selection guide for the TTGO T-Watch 2019/2020 PCB assembly. It includes all critical components, their Part Numbers (PN), suppliers, and procurement considerations.

## Core Components

### 1. Microcontroller Unit (MCU)

| Attribute | Value |
|-----------|-------|
| **Component** | ESP32-WROB-BW |
| **Package** | QFN-48 (6x6mm) |
| **Manufacturer** | Espressif Systems |
| **Key Features** | Dual-core 240MHz, WiFi + BLE, 520KB SRAM |
| **Supplier Part Numbers** | ESP32-WROB-BW (Espressif), ESP32-WROOM-32 (Wroga) |
| **Alternate Sources** | DigiKey, Mouser, Arrow, LCSC |
| **Typical Cost** | $2.50 - $4.00 USD |

**Procurement Notes:**
- ESP32-WROB-BW is the recommended variant for better availability
- Verify RoHS compliance when ordering
- Check batch lot for consistent RF performance

### 2. Power Management IC (PMIC)

| Attribute | Value |
|-----------|-------|
| **Component** | AXP20X (AXP202 or AXP2101) |
| **Package** | UFL-54 (5.65x6.35mm) |
| **Manufacturer** | Advanced X-Power (AXP) |
| **Key Features** | LDOs, DC-DC converters, fuel gauge, charging management |
| **Supplier Part Numbers** | AXP202 (older), AXP2101 (newer, recommended) |
| **Alternate Sources** | Available through module suppliers (TTGO, LilyGO) |
| **Typical Cost** | $1.50 - $2.50 USD (standalone chip harder to source) |

**Procurement Notes:**
- AXP2101 is preferred for newer designs (better efficiency)
- Standalone sourcing is challenging - often sold as part of module
- Consider using pre-integrated TTGO module for reliability

### 3. Display Driver

| Attribute | Value |
|-----------|-------|
| **Component** | ST7789V |
| **Package** | QFN-48 (5x5mm) |
| **Manufacturer** | SITronix (ST Micro) |
| **Key Features** | 240x240 RGB LCD driver, SPI interface |
| **Supplier Part Numbers** | ST7789VYQ (QFN-48), ST7789S (alternative) |
| **Alternate Sources** | DigiKey, Mouser, LCSC |
| **Typical Cost** | $1.00 - $2.00 USD |

**Procurement Notes:**
- ST7789V is standard for 1.54" 240x240 displays
- Compatible with LVGL graphics library
- SPI interface reduces MCU pin usage

### 4. Touch Controller

| Attribute | Value |
|-----------|-------|
| **Component** | FT6236 |
| **Package** | QFN-20 (3x3mm) |
| **Manufacturer** | FocalTech (Panasonic) |
| **Key Features** | Capacitive touch, I2C interface, 6-touch support |
| **Supplier Part Numbers** | FT6236, FT6236U |
| **Alternate Sources** | DigiKey, Mouser, LCSC |
| **Typical Cost** | $1.50 - $2.50 USD |

**Procurement Notes:**
- FT6236 supports up to 6 simultaneous touches
- I2C interface compatible with ESP32
- Driver available in Arduino TFT_eSPI library

### 5. Accelerometer

| Attribute | Value |
|-----------|-------|
| **Component** | BMA423 |
| **Package** | LGA-14 (2.0x2.5mm) |
| **Manufacturer** | Bosch Sensortec |
| **Key Features** | 3-axis accelerometer, step counter, gesture detection |
| **Supplier Part Numbers** | BMA423 (Bosch), BMA421 (alternate) |
| **Alternate Sources** | DigiKey, Mouser, Arrow, LCSC |
| **Typical Cost** | $2.00 - $3.50 USD |

**Procurement Notes:**
- Ultra-low power consumption (~0.5μA standby)
- Integrated step counter reduces MCU load
- Bosch provides Arduino library (BMA423)

### 6. RTC Battery

| Attribute | Value |
|-----------|-------|
| **Component** | CR2032 |
| **Package** | Button Cell |
| **Manufacturer** | Multiple (Panasonic, Maxell, Sony) |
| **Key Features** | 3V lithium coin cell, 220mAh capacity |
| **Supplier Part Numbers** | CR2032 (standard), BR2032 (temperature variant) |
| **Alternate Sources** | DigiKey, Mouser, Amazon, Local electronics stores |
| **Typical Cost** | $0.50 - $1.50 USD |

**Procurement Notes:**
- Standard CR2032 for most applications
- BR2032 for extended temperature range (-40°C to +85°C)
- Replaceable battery design for continuous RTC operation

### 7. Audio Codec

| Attribute | Value |
|-----------|-------|
| **Component** | MAX98357A |
| **Package** | WLCSP-7 |
| **Manufacturer** | Maxim Integrated (Analog Devices) |
| **Key Features** | Class-D amplifier, I2S interface, 3W output |
| **Supplier Part Numbers** | MAX98357AETA+ |
| **Alternate Sources** | DigiKey, Mouser, Arrow |
| **Typical Cost** | $1.50 - $2.50 USD |

**Procurement Notes:**
- Supports BLE audio streaming
- I2S interface from ESP32
- 3W mono output for speaker

### 8. GPS Module (Optional)

| Attribute | Value |
|-----------|-------|
| **Component** | AIR530 / u-blox MAX-M10S |
| **Package** | Surface Mount |
| **Manufacturer** | SiRF (Air530) / u-blox (M10S) |
| **Key Features** | GNSS receiver, L1 band, I2C/SPI interface |
| **Supplier Part Numbers** | AIR530 (SiRF), MAX-M10S (u-blox) |
| **Alternate Sources** | DigiKey, Mouser, u-blox distributors |
| **Typical Cost** | $8.00 - $15.00 USD |

**Procurement Notes:**
- TTGO T-Watch 2020 includes GPS variant
- Air530 is legacy, M10S is newer replacement
- Consider antenna placement for signal quality

## Passive Components

### Resistors

| Value | Quantity | Package | Supplier |
|-------|----------|---------|----------|
| 0Ω | 4 | 0402 | Yageo, Murata |
| 10kΩ | 6 | 0402 | Yageo, Murata |
| 100kΩ | 3 | 0402 | Yageo, Murata |
| 220Ω | 2 | 0402 | Yageo, Murata |

### Capacitors

| Value | Quantity | Package | Type | Supplier |
|-------|----------|---------|------|----------|
| 100pF | 4 | 0402 | C0G/NP0 | Murata, TDK |
| 1nF | 6 | 0402 | X7R | Murata, TDK |
| 10nF | 12 | 0402 | X7R | Murata, TDK |
| 100nF | 8 | 0402 | X7R | Murata, TDK |
| 10μF | 3 | 0603 | X5R | Murata, TDK |

### Inductors

| Value | Quantity | Package | Supplier |
|-------|----------|---------|----------|
| 4.7μH | 1 | 1210 | TDK, Murata |

## Connectors & Interfaces

### 1. USB Interface

| Attribute | Value |
|-----------|-------|
| **Component** | USB Type-C |
| **Package** | Surface Mount 16-pin |
| **Manufacturer** | Multiple (JST, Hirose, Taitek) |
| **Key Features** | USB 2.0, charging support |
| **Supplier Part Numbers** | USBC-16SMT (generic), 10321711-0001T (Taitek) |
| **Alternate Sources** | DigiKey, Mouser, JST America |
| **Typical Cost** | $0.50 - $1.50 USD |

### 2. Speaker Connector

| Attribute | Value |
|-----------|-------|
| **Component** | 2-pin Header |
| **Package** | Through-hole or SMT |
| **Manufacturer** | JST, Molex |
| **Key Features** | 2.0mm pitch for speaker cable |
| **Supplier Part Numbers** | B2B-PH-K-S (JST) |
| **Typical Cost** | $0.10 - $0.30 USD |

### 3. Button Connectors

| Attribute | Value |
|-----------|-------|
| **Component** | 3x2 button array |
| **Package** | Tactile switch |
| **Manufacturer** | Omron, Kailh |
| **Key Features** | 6mm x 6mm, 12mm pitch |
| **Supplier Part Numbers** | VMP04 (Omron), B3FS (Omron) |
| **Typical Cost** | $0.05 - $0.15 USD each |

## Display Assembly

| Attribute | Value |
|-----------|-------|
| **Component** | 1.54" IPS LCD |
| **Resolution** | 240x240 pixels |
| **Supplier** | Tianma, BOE, ILITEK |
| **Display Driver** | ST7789 integrated |
| **Touch Sensor** | FT6236 (capacitive) |
| **Typical Cost** | $12.00 - $18.00 USD |

## PCB Specifications

### Board Material
- **Substrate**: FR-4, 1.6mm thickness
- **Copper**: 1oz (35μm)
- **Layers**: 2-layer board
- **Solder Mask**: Green (or custom color)
- **Silkscreen**: White

### Board Dimensions
- **Size**: 52mm x 52mm (square)
- **Mounting Holes**: 4x M2.5

## Sourcing Recommendations

### Primary Distributors (Recommended)
1. **DigiKey** - Wide selection, fast shipping
2. **Mouser** - Good for Espressif, Bosch components
3. **LCSC** - Cost-effective, good for passive components
4. **Arrow Electronics** - Volume orders, authorized dealer

### Alternative Sources
1. **AliExpress** - TTGO modules (pre-assembled)
2. **JLCPCB** - PCB assembly service
3. **PCBWay** - PCB manufacturing + assembly
4. **Seeed Studio** - Fusion PCB service

### Cost Optimization
- Order passives in 10k reel quantities
- Consider pre-assembled TTGO modules for prototyping
- Use JLCPCB/PCBWay for low-volume assembly (<100 units)

## Quality Assurance

### Component Verification
- Check IC markings and lot numbers
- Verify capacitor voltage ratings
- Test ESP32 RF performance (optional)
- Measure AXP20X output voltages

### Assembly Guidelines
- Reflow profile: 217°C peak (SN100C solder)
- Check for tombstoning on 0402 components
- Verify USB-C solder joints
- Inspect QFN thermal pads for solder bridges

### Testing Checklist
1. USB power-up (5V, 500mA)
2. Display initialization (LVGL test)
3. Touch calibration
4. Accelerometer I2C communication
5. AXP20X voltage regulation
6. BLE connectivity
7. WiFi RSSI measurement
8. Battery charging function

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-03-22 | Initial release - TTGO T-Watch 2019/2020 parts guide |

## References

- [TTGO T-Watch Product Page](https://www.seeedstudio.com/TTGO-T-Watch.html)
- [Espressif ESP32 Datasheet](https://www.espressif.com/en/products/specs/ESP32-WROB-BW/view)
- [Bosch BMA423 Datasheet](https://www.bosch-sensortec.com/products/motion-sensors/bma423/)
- [AXP2101 Datasheet](https://www.innoscience.com/uploads/AXP2101_DS_V1.0_20220915.pdf)
- [ST7789 Datasheet](https://www.sitronix.cn/uploads/file/20210913/202109131438178720.pdf)

## Appendix A: Bill of Materials (BOM) Summary

### Active Components (12 total)
1. ESP32-WROB-BW - 1x
2. AXP2101 - 1x
3. ST7789V - 1x
4. FT6236 - 1x
5. BMA423 - 1x
6. MAX98357A - 1x
7. AIR530 - 1x (GPS variant only)

### Passive Components (~47 total)
- Resistors (0402): 15x
- Capacitors (0402/0603): 30x
- Inductors (1210): 1x

### Connectors (5 total)
- USB Type-C: 1x
- Speaker JST PH 2.0mm: 1x
- 3x2 Button Array: 1x
- Battery Connector: 1x
- IPEX GPS Antenna: 1x

### Display (1 total)
- 1.54" 240x240 LCD + Touch: 1x

### Battery (1 total)
- CR2032 Coin Cell: 1x

**Total BOM Cost (Prototype Quantities): ~$35-45 USD**
**Total BOM Cost (1000 units): ~$18-22 USD**
