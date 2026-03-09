#include "BatteryMonitor.h"

#ifdef NATIVE_BUILD
// Simulation for native testing
static int _sim_percentage = 100;
static bool _sim_charging = false;
static bool _sim_usb = false;
static int _sim_update_count = 0;
#else
#include <Arduino.h>
// AXP20X power management would be included here
// #include "axp20x.h"
#endif

BatteryMonitor::BatteryMonitor()
    : _initialized(false), _alertCallback(nullptr), _batteryWidget(nullptr),
      _lastLowAlert(false), _lastCriticalAlert(false), _updateCount(0) {}

BatteryMonitor::~BatteryMonitor() { deinit(); }

bool BatteryMonitor::init() {
    if (_initialized) return true;
#ifdef NATIVE_BUILD
    _sim_percentage = 100;
    _sim_charging = false;
    _sim_usb = false;
    _sim_update_count = 0;
#endif
    _status.percentage = 100;
    _status.voltage = 4.2f;
    _lastLowAlert = false;
    _lastCriticalAlert = false;
    _initialized = true;
    return true;
}

void BatteryMonitor::deinit() {
    _initialized = false;
}

void BatteryMonitor::update() {
    if (!_initialized) return;
    _updateCount++;
    _readBatteryData();
    _checkAlerts();
    _updatePowerSaving();
}

int BatteryMonitor::getPercentage() const { return _status.percentage; }
float BatteryMonitor::getVoltage() const { return _status.voltage; }
bool BatteryMonitor::isCharging() const { return _status.isCharging; }
bool BatteryMonitor::isUSBConnected() const { return _status.isUSBConnected; }
float BatteryMonitor::getTemperature() const { return _status.temperature; }
BatteryMonitor::Status BatteryMonitor::getStatus() const { return _status; }

void BatteryMonitor::setConfig(const Config& config) { _config = config; }
const BatteryMonitor::Config& BatteryMonitor::getConfig() const { return _config; }

void BatteryMonitor::setAlertCallback(AlertCallback callback) { _alertCallback = callback; }
bool BatteryMonitor::isLowBattery() const { return _status.isLowBattery; }
bool BatteryMonitor::isCritical() const { return _status.isCritical; }

void BatteryMonitor::enablePowerSaving(bool enable) { _config.enablePowerSaving = enable; }
bool BatteryMonitor::isPowerSavingActive() const { return _status.powerSavingActive; }

void BatteryMonitor::setBatteryWidget(void* widget) { _batteryWidget = widget; }

void BatteryMonitor::_readBatteryData() {
#ifdef NATIVE_BUILD
    _sim_update_count++;
    if (_sim_charging) {
        _sim_percentage += 2;
        if (_sim_percentage > 100) _sim_percentage = 100;
    } else {
        _sim_percentage -= 1;
        if (_sim_percentage < 0) _sim_percentage = 0;
    }
    // Simulate USB toggle at 200 updates
    if (_sim_update_count == 200) {
        _sim_usb = !_sim_usb;
        _sim_charging = _sim_usb;
    }
    _status.percentage = _sim_percentage;
    _status.voltage = 3.0f + (_sim_percentage / 100.0f) * 1.2f;
    _status.isCharging = _sim_charging;
    _status.isUSBConnected = _sim_usb;
    _status.temperature = 25.0f;
    _status.dischargeCurrent = _sim_charging ? 0.0f : 150.0f;
#else
    // Real AXP20X reads would go here
    // _status.voltage = axp.getBattVoltage() / 1000.0f;
    // _status.percentage = axp.getBattPercentage();
    // _status.isCharging = axp.isChargeing();
    // etc.
#endif
}

void BatteryMonitor::_checkAlerts() {
    bool nowLow = _status.percentage <= _config.lowBatteryThreshold;
    bool nowCritical = _status.percentage <= _config.criticalThreshold;

    _status.isLowBattery = nowLow;
    _status.isCritical = nowCritical;

    if (!_config.enableAlerts || !_alertCallback) return;

    // Edge-triggered: only fire on transition
    if (nowCritical && !_lastCriticalAlert) {
        _alertCallback("critical", _status.percentage);
    } else if (nowLow && !_lastLowAlert) {
        _alertCallback("low_battery", _status.percentage);
    }

    _lastLowAlert = nowLow;
    _lastCriticalAlert = nowCritical;
}

void BatteryMonitor::_updatePowerSaving() {
    if (_config.enablePowerSaving && _status.isLowBattery && !_status.isCharging) {
        _status.powerSavingActive = true;
    } else {
        _status.powerSavingActive = false;
    }
}
