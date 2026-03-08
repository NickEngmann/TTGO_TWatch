#include "BatteryMonitor.h"

#ifdef NATIVE_BUILD
#include <cstdlib>
#else
#include "TTGO.h"
#endif

BatteryMonitor::BatteryMonitor()
    : _config{}
    , _status{}
    , _initialized(false)
    , _alertCallback(nullptr)
    , _lvglWidget(nullptr)
    , _updateCount(0)
    , _prevLowBattery(false)
    , _prevCritical(false)
{
}

BatteryMonitor::~BatteryMonitor() {
    deinit();
}

bool BatteryMonitor::init() {
#ifdef NATIVE_BUILD
    _status.percentage = 85;
    _status.voltage = 4.05f;
    _status.temperature = 25.0f;
    _status.dischargeCurrent = 120.0f;
    _status.isCharging = false;
    _status.isUSBConnected = false;
    _status.isLowBattery = false;
    _status.isCritical = false;
    _status.powerSavingActive = false;
    _status.initialized = true;
    _initialized = true;
    _updateCount = 0;
    _prevLowBattery = false;
    _prevCritical = false;
    return true;
#else
    // Initialize AXP20X power management
    _status.initialized = true;
    _initialized = true;
    _updateCount = 0;
    _prevLowBattery = false;
    _prevCritical = false;
    _readBatteryData();
    return true;
#endif
}

void BatteryMonitor::deinit() {
    _initialized = false;
    _status.initialized = false;
}

void BatteryMonitor::_readBatteryData() {
    if (!_initialized) return;

#ifdef NATIVE_BUILD
    _updateCount++;

    // Simulate USB connection every 30 updates (toggles)
    if (_updateCount > 0 && (_updateCount % 200) == 0) {
        _status.isUSBConnected = !_status.isUSBConnected;
    }

    // Simulate charging when USB connected
    _status.isCharging = _status.isUSBConnected;

    if (_status.isCharging) {
        // Charge: gain 2% per update, cap at 100
        _status.percentage += 2;
        if (_status.percentage > 100) _status.percentage = 100;
    } else {
        // Drain: lose 1% per update, floor at 0
        _status.percentage -= 1;
        if (_status.percentage < 0) _status.percentage = 0;
    }

    // Derive voltage from percentage (3.0V empty, 4.2V full)
    _status.voltage = 3.0f + (_status.percentage / 100.0f) * 1.2f;

    // Simulate temperature (slight increase with discharge)
    _status.temperature = 25.0f + (100 - _status.percentage) * 0.05f;

    // Simulate discharge current
    _status.dischargeCurrent = _status.isCharging ? -500.0f : 120.0f;
#else
    TTGOClass *ttgo = TTGOClass::getWatch();
    _status.percentage = (int)ttgo->power->getBattPercentage();
    _status.voltage = ttgo->power->getBattVoltage() / 1000.0f;
    _status.isCharging = ttgo->power->isChargeing();
    _status.isUSBConnected = ttgo->power->isVBUSPlug();
    _status.temperature = ttgo->power->getTemp();
    _status.dischargeCurrent = ttgo->power->getBattDischargeCurrent();
#endif

    // Update derived flags
    _status.isLowBattery = (_status.percentage <= _config.lowBatteryThreshold);
    _status.isCritical = (_status.percentage <= _config.criticalThreshold);
}

void BatteryMonitor::update() {
    if (!_initialized) return;

    _readBatteryData();
    _checkAlerts();
    _updatePowerSaving();
    _updateLVGLWidget();
}

void BatteryMonitor::_checkAlerts() {
    if (!_config.enableAlerts || !_alertCallback) return;

    // Fire alert on crossing into critical (edge trigger)
    if (_status.isCritical && !_prevCritical) {
        _alertCallback("critical", _status.percentage);
    }
    // Fire alert on crossing into low battery (edge trigger)
    else if (_status.isLowBattery && !_prevLowBattery) {
        _alertCallback("low_battery", _status.percentage);
    }

    _prevLowBattery = _status.isLowBattery;
    _prevCritical = _status.isCritical;
}

void BatteryMonitor::_updatePowerSaving() {
    if (!_config.enablePowerSaving) {
        _status.powerSavingActive = false;
        return;
    }

    if (_status.isLowBattery && !_status.isCharging) {
        _status.powerSavingActive = true;
#ifndef NATIVE_BUILD
        // Reduce backlight
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->bl->adjust(50);
#endif
    } else {
        _status.powerSavingActive = false;
    }
}

void BatteryMonitor::_updateLVGLWidget() {
    if (_lvglWidget == nullptr) return;

#ifdef NATIVE_BUILD
    // Native: no-op, widget pointer stored for testing
    (void)_lvglWidget;
#else
    // Update LVGL bar widget with battery percentage
    lv_obj_t* bar = (lv_obj_t*)_lvglWidget;
    lv_bar_set_value(bar, _status.percentage, LV_ANIM_OFF);
#endif
}

BatteryMonitor::Status BatteryMonitor::getStatus() const {
    return _status;
}

int BatteryMonitor::getPercentage() const {
    return _status.percentage;
}

float BatteryMonitor::getVoltage() const {
    return _status.voltage;
}

bool BatteryMonitor::isCharging() const {
    return _status.isCharging;
}

void BatteryMonitor::setConfig(const Config& config) {
    _config = config;
    // Re-evaluate alerts with new thresholds
    _status.isLowBattery = (_status.percentage <= _config.lowBatteryThreshold);
    _status.isCritical = (_status.percentage <= _config.criticalThreshold);
}

BatteryMonitor::Config BatteryMonitor::getConfig() const {
    return _config;
}

void BatteryMonitor::setAlertCallback(AlertCallback callback) {
    _alertCallback = callback;
}

void BatteryMonitor::enablePowerSaving(bool enable) {
    _config.enablePowerSaving = enable;
    if (!enable) {
        _status.powerSavingActive = false;
    }
}

void BatteryMonitor::setLVGLWidget(void* widget) {
    _lvglWidget = widget;
}
