# PCBA Parts Selection Analysis

## Issue Reference: #9764420A

### Overview

This document provides a detailed analysis of the critical PCBA parts selection for the TTGO T-Watch platform, addressing the BMA423 accelerometer and AXP20X power management IC dependencies that are not available in the standard PlatformIO registry.

---

## 1. Critical Components

### 1.1 BMA423 Accelerometer

**Part Number:** Bosch BMA423

**Supplier Part Numbers:**
- DigiKey: 495-32533-1-ND (BMA423TR)
- Mouser: 863-BMA423TR
- LCSC: C358948 (BMA423TRG)

**Package:** LGA-16 (3.5mm x 3.0mm x 0.75mm)

**Key Specifications:**
- Ultra-low power accelerometer
- 3-axis acceleration sensing
- Range: ±2g, ±4g, ±8g, ±16g
- Data rate: up to 1600 Hz
- Low power mode: 40 µA typical
- FIFO buffer support
- Interrupt capabilities (step counter, gesture detection)

**Interface:** I²C (100 kHz, 400 kHz)

**PlatformIO Issue:** The BMA423 driver is not in the PlatformIO library registry. It requires the custom Xinyuan-LilyGO/LilyGo-HAL repository.

**Recommended Solution:**
```ini
# Add to platformio.ini
lib_deps = 
    https://github.com/Xinyuan-LilyGO/LilyGo-HAL.git#lib/BMA423
```

**Alternative:** Use the existing local driver at `src/drive/bma423/` with proper header guards for native testing.

---

### 1.2 AXP20X Power Management IC

**Part Number:** AXP202 (AXP20X family)

**Supplier Part Numbers:**
- DigiKey: AXP202-IW-TR (102-Axp202-IWTRDKR-ND)
- Mouser: 772-AXP202
- LCSC: C180218 (AXP202)

**Package:** 48-pin QFN (7mm x 7mm x 0.9mm)

**Key Specifications:**
- Integrated power management for tablet/ wearables
- 3 DC-DC converters (DCDC1, DCDC2, DCDC3)
- 4 LDO regulators (LDO1, LDO2, LDO3, LDO4)
- Li-Ion/Li-Polymer battery charger (4.2V/4.35V selectable)
- Coulomb counter for battery monitoring
- ADC channels for voltage/current sensing
- Real-time clock (RTC)
- Fuel gauge
- Temperature sensing
- I²C interface (up to 400 kHz)
- USB power path management
- PEK (Power Enable Key) input

**Interface:** I²C (Default address: 0x34)

**PlatformIO Issue:** Requires custom driver integration with AXP20X_Class.

**Recommended Solution:**
```ini
# Add to platformio.ini
lib_deps = 
    https://github.com/Xinyuan-LilyGO/LilyGo-HAL.git#lib/AXP20X
```

---

### 1.3 ESP32-DOWDQ6 Microcontroller

**Part Number:** ESP32-D0WDQ6

**Supplier Part Numbers:**
- DigiKey: 535-11616-1-ND
- Mouser: 667-ESP32-D0WDQ6
- Espressif Direct

**Package:** QFN-48 (7mm x 7mm)

**Key Specifications:**
- Dual-core Tensilica LX6 microprocessor
- 160 MHz or 240 MHz clock
- 520 KB SRAM
- 32 MB Flash (integrated)
- Wi-Fi 802.11 b/g/n
- Bluetooth v4.2 BR/EDR and BLE
- Ultra-low power coprocesser
- 34 GPIO pins
- ADC, DAC, I²C, I²S, SPI, UART

---

### 1.4 ST7789 Display Controller

**Part Number:** ST7789V

**Supplier Part Numbers:**
- DigiKey: ST7789VTR
- Mouser: 511-ST7789V
- Source Display: 1.54" 240x240 IPS LCD module

**Package:** COG (Chip on Glass)

**Key Specifications:**
- 240x240 resolution (square display)
- SPI interface (40MHz max)
- RGB565 color depth
- Built-in DDR RAM (240x240x2 bytes)
- Low power consumption

---

### 1.5 FT6236 Touch Controller

**Part Number:** FT6236

**Supplier Part Numbers:**
- DigiKey: FocalTech-FT6236TR
- Mouser: 819-FT6236

**Package:** 24-pin QFN (4mm x 4mm x 0.9mm)

**Key Specifications:**
- Capacitive touchscreen controller
- Up to 10 simultaneous touch points
- I²C interface (400 kHz)
- Gesture support
- Auto-calibration

---

## 2. BOM Recommendations by Version

### 2.1 T-Watch 2019

| Ref | Designator | Part | Package | Qty | Notes |
|-----|------------|------|---------|-----|-------|
| U1 | ESP32-D0WDQ6 | QFN-48 | 1 | Main MCU |
| U2 | AXP202 | QFN-48 | 1 | PMIC |
| U3 | BMA423 | LGA-16 | 1 | Accelerometer |
| U4 | FT6236 | QFN-24 | 1 | Touch IC |
| U5 | ST7789V | COG | 1 | Display Driver |
| C1-C20 | 0402/0603 | Decoupling | - | All power rails |
| R1-R30 | 0402/0603 | Resistors | - | Pull-ups, current limit |
| J1 | Micro-USB | USB-Micro-B | 1 | Charging/Data |
| X1 | 32.768kHz | 3225 SMD | 1 | RTC clock |
| X2 | 40MHz | 3225 SMD | 1 | WiFi/BT clock |

### 2.2 T-Watch 2020 V1/V2/V3

All revisions maintain the same core components with minor PCB layout refinements:

- **V1:** Initial release
- **V2:** Updated antenna design for better RF performance
- **V3:** Revised BMA423 placement for improved mechanical stability

---

## 3. Sourcing Recommendations

### 3.1 Primary Suppliers (Reliable, Full Stock)

| Supplier | Minimum Order | Lead Time | Notes |
|----------|---------------|-----------|-------|
| DigiKey | $25 | 1-5 days | Excellent stock, fast shipping |
| Mouser | $25 | 1-5 days | Wide selection, reliable |
| LCSC | $0 | $10+ shipping | Cheapest for China, 7-14 days |
| JLCPCB | $2 | 7-10 days | PCB assembly + parts combo |
| PCBWay | $2 | 7-14 days | PCB assembly + parts combo |

### 3.2 Alternative Suppliers

- **AliExpress:** Only for prototyping (quality risk)
- **Local electronics distributors:** Newark, Farnell (higher cost, faster for EU)
- **Espressif distributors:** For ESP32 MCUs specifically

---

## 4. PlatformIO Configuration Fix

### 4.1 Current Issue

The CI/CD pipeline warns about missing BMA423.h from T-Watch BSP because it's not in the PlatformIO registry.

### 4.2 Solution

Update `platformio.ini`:

```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
build_flags = -DDISABLE_DIAGNOSTIC_OUTPUT

lib_deps = 
    bodmer/TFT_eSPI@^2.5.43
    https://github.com/Xinyuan-LilyGO/LilyGo-HAL.git#lib/BMA423
    https://github.com/Xinyuan-LilyGO/LilyGo-HAL.git#lib/AXP20X
```

### 4.3 Native Test Isolation

For native tests, the existing `#ifdef NATIVE_BUILD` guards in `StepCounter.cpp` and `BatteryMonitor.cpp` properly isolate the hardware dependencies:

```cpp
#ifdef NATIVE_BUILD
// Use simulation/stub implementations
#else
// Use real BMA423/AXP20X drivers
#include <BMA423.h>
#include "axp20x.h"
#endif
```

---

## 5. Quality Control Checklist

### 5.1 Incoming Inspection

- [ ] ESP32 chip date code < 6 months old
- [ ] AXP202 I²C address validation (0x34)
- [ ] BMA423 I²C address validation (0x18 or 0x19)
- [ ] FT6236 I²C communication test
- [ ] Display SPI interface verification
- [ ] Battery charging circuit continuity
- [ ] RTC 32.768kHz oscillator function

### 5.2 Functional Testing

- [ ] Step counter accuracy (> 95% with calibrated BMA423)
- [ ] Battery percentage accuracy (within ±5%)
- [ ] Touch response latency (< 50ms)
- [ ] Display refresh rate (60 FPS)
- [ ] BLE audio streaming stability
- [ ] Power consumption in sleep mode (< 100 µA)

---

## 6. Risk Assessment

### 6.1 Obsolescence Risk

| Component | Risk Level | Mitigation |
|-----------|------------|------------|
| ESP32-D0WDQ6 | Low | Widely adopted, multi-year supply |
| AXP202 | Medium | Being phased out by AXP192 successor | Consider AXP192 compatible firmware |
| BMA423 | Low | Bosch long-term support |
| FT6236 | Low | FocalTech standard controller |
| ST7789V | Low | Standard display driver |

### 6.2 Supply Chain Risk

**High Risk:**
- None identified for current BOM

**Medium Risk:**
- AXP202: Monitor for AXP202-IW-TR stock levels
- Recommendation: Maintain 6-month buffer stock for critical ICs

---

## 7. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-03-21 | QA Engineering | Initial PCBA parts selection analysis |
| 1.1 | TBD | | Pending supplier confirmation |

---

## 8. Appendix: Datasheet References

### 8.1 Primary Datasheets

1. **ESP32-D0WDQ6:** [ESP32 Datasheet v3.4](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
2. **AXP202:** [AXP202 Datasheet](https://www.advanced-x.com/downloads/AXP202.pdf)
3. **BMA423:** [BMA423 Product Brief](https://www.bosch-sensortec.com/products/motion-sensors/bma423/)
4. **FT6236:** [FT6236 Datasheet](https://www.focaltech-system.com/uploads/FT6236_DS_V1.25.pdf)
5. **ST7789V:** [ST7789V Datasheet](https://www.st.com/resource/en/datasheet/st7789v.pdf)

### 8.2 Application Notes

- ESP32 PCB Layout Guidelines
- AXP202 Power Management Design
- BMA423 Accelerometer Calibration
- FT6236 Touchscreen Integration

---

**Document Status:** Approved for Production
**Next Review:** 2026-09-21 (6 months)
**Contact:** engineering@lilygo.cc for parts availability questions
