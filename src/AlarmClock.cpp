#include "AlarmClock.h"

#ifdef NATIVE_BUILD
#include <cstring>
#include <cstdlib>
#else
#include "TTGO.h"
#endif

AlarmClock::AlarmClock()
    : _initialized(false)
    , _pendingAlarmIndex(255)
    , _hasPendingAlarm(false)
    , _alarmCallback(nullptr)
#ifdef NATIVE_BUILD
    , _currentTimeHour(0)
    , _currentTimeMinute(0)
#endif
{
    // Default config
    _config.autoCheckEnabled = true;
    _config.checkIntervalMs = 1000;
    _config.playTone = true;
    _config.vibrate = true;
}

AlarmClock::~AlarmClock() {
    deinit();
}

bool AlarmClock::init() {
#ifdef NATIVE_BUILD
    // Simulate initialization for native builds
    _initialized = true;
    _currentTimeHour = 0;
    _currentTimeMinute = 0;
    return true;
#else
    // Initialize PCF8563 RTC if not already initialized
    _rtc = PCF8563;
    if (_rtc == nullptr) {
        return false;
    }
    
    _initialized = true;
    _clearPCF8563Alarm();
    return true;
#endif
}

void AlarmClock::deinit() {
#ifdef NATIVE_BUILD
    _alarms.clear();
    _initialized = false;
    _hasPendingAlarm = false;
    _pendingAlarmIndex = 255;
#else
    if (_initialized && _rtc != nullptr) {
        _clearPCF8563Alarm();
        _initialized = false;
    }
#endif
}

void AlarmClock::update() {
    if (!_initialized || !_config.autoCheckEnabled) {
        return;
    }

#ifdef NATIVE_BUILD
    // In native build, simulate time progression for testing
    // Time advances 1 minute every 100 iterations for testing purposes
    static uint32_t simulationCount = 0;
    simulationCount++;
    
    _currentTimeMinute++;
    if (_currentTimeMinute >= 60) {
        _currentTimeMinute = 0;
        _currentTimeHour++;
        if (_currentTimeHour >= 24) {
            _currentTimeHour = 0;
        }
    }
    
    // Check for alarms at the current time
    uint8_t activeAlarm = _findFirstEnabledAlarm(_currentTimeHour, _currentTimeMinute);
    if (activeAlarm != 255) {
        _handleAlarmFired(activeAlarm);
    }
#else
    // Get current time from RTC
    uint8_t hour, minute;
    _rtc->getTime(&hour, &minute, nullptr);
    
    // Check for alarms
    uint8_t activeAlarm = _findFirstEnabledAlarm(hour, minute);
    if (activeAlarm != 255) {
        _handleAlarmFired(activeAlarm);
    }
#endif
}

uint8_t AlarmClock::addAlarm(uint8_t hour, uint8_t minute, const char* label, uint32_t repeatDays) {
    if (!_initialized) {
        return 255;
    }
    
    // Validate inputs
    if (hour > 23 || minute > 59) {
        return 255;
    }
    
    AlarmEntry entry;
    entry.hour = hour;
    entry.minute = minute;
    entry.enabled = true;
    entry.repeatDays = repeatDays;
    entry.label[0] = '\0';
    
    if (label != nullptr) {
        strncpy(entry.label, label, sizeof(entry.label) - 1);
        entry.label[sizeof(entry.label) - 1] = '\0';
    }
    
    _alarms.push_back(entry);
    _setupPCF8563Alarm(_alarms.size() - 1);
    
    return static_cast<uint8_t>(_alarms.size() - 1);
}

bool AlarmClock::removeAlarm(uint8_t index) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    _alarms[index].enabled = false;
    _setupPCF8563Alarm(index);
    
    // Remove from vector if it's the last alarm
    if (index == _alarms.size() - 1) {
        _alarms.pop_back();
    } else {
        // Mark as disabled and shift remaining
        _alarms.erase(_alarms.begin() + index);
    }
    
    return true;
}

bool AlarmClock::enableAlarm(uint8_t index) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    _alarms[index].enabled = true;
    _setupPCF8563Alarm(index);
    return true;
}

bool AlarmClock::disableAlarm(uint8_t index) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    _alarms[index].enabled = false;
    _setupPCF8563Alarm(index);
    return true;
}

bool AlarmClock::toggleAlarm(uint8_t index) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    _alarms[index].enabled = !_alarms[index].enabled;
    _setupPCF8563Alarm(index);
    return true;
}

bool AlarmClock::setAlarmTime(uint8_t index, uint8_t hour, uint8_t minute) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    if (hour > 23 || minute > 59) {
        return false;
    }
    
    _alarms[index].hour = hour;
    _alarms[index].minute = minute;
    _setupPCF8563Alarm(index);
    return true;
}

bool AlarmClock::setAlarmLabel(uint8_t index, const char* label) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    if (label != nullptr) {
        strncpy(_alarms[index].label, label, sizeof(_alarms[index].label) - 1);
        _alarms[index].label[sizeof(_alarms[index].label) - 1] = '\0';
    } else {
        _alarms[index].label[0] = '\0';
    }
    
    return true;
}

bool AlarmClock::setAlarmRepeat(uint8_t index, uint32_t repeatDays) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    _alarms[index].repeatDays = repeatDays;
    return true;
}

AlarmEntry AlarmClock::getAlarm(uint8_t index) const {
    if (!_initialized || index >= _alarms.size()) {
        return {};
    }
    
    return _alarms[index];
}

uint8_t AlarmClock::getAlarmCount() const {
    return static_cast<uint8_t>(_alarms.size());
}

bool AlarmClock::isAlarmActive(uint8_t index) const {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    return _alarms[index].enabled;
}

bool AlarmClock::checkAlarm(uint8_t index, uint8_t currentHour, uint8_t currentMinute) {
    if (!_initialized || index >= _alarms.size()) {
        return false;
    }
    
    const AlarmEntry& alarm = _alarms[index];
    if (!alarm.enabled) {
        return false;
    }
    
    // Check if alarm time matches current time
    if (alarm.hour != currentHour || alarm.minute != currentMinute) {
        return false;
    }
    
    // Check repeat pattern
    if (alarm.repeatDays > 0) {
        // For now, we assume repeatDays is handled by the user
        // In a full implementation, we'd check the current day of week
    }
    
    return true;
}

void AlarmClock::setAlarmCallback(AlarmClock::AlarmCallback callback) {
    _alarmCallback = callback;
}

bool AlarmClock::hasPendingAlarm() const {
    return _hasPendingAlarm;
}

uint8_t AlarmClock::getPendingAlarmIndex() const {
    return _pendingAlarmIndex;
}

void AlarmClock::clearPendingAlarm() {
    _hasPendingAlarm = false;
    _pendingAlarmIndex = 255;
}

void AlarmClock::setConfig(const Config& config) {
    _config = config;
}

const AlarmClock::Config& AlarmClock::getConfig() const {
    return _config;
}

bool AlarmClock::hasAlarmOccurred() {
#ifdef NATIVE_BUILD
    return _hasPendingAlarm;
#else
    if (_rtc == nullptr) {
        return false;
    }
    return _rtc->alarmActive();
#endif
}

AlarmEventType AlarmClock::getAlarmType() {
#ifdef NATIVE_BUILD
    return AlarmEventType::MINUTE_ALARM;
#else
    if (_rtc == nullptr) {
        return AlarmEventType::NO_ALARM;
    }
    
    uint8_t status = 0;
    _rtc->readStatusReg(status);
    
    if (status & PCF8563_ALARM_AF) {
        // Check alarm type from alarm register
        uint8_t alarmReg = 0;
        _rtc->readAlarmReg(alarmReg);
        // Simplified - in real code would check which bits are set
        return AlarmEventType::MINUTE_ALARM;
    }
    
    return AlarmEventType::NO_ALARM;
#endif
}

bool AlarmClock::_setupPCF8563Alarm(uint8_t index) {
#ifdef NATIVE_BUILD
    (void)index;  // Unused in native build
    return true;
#else
    if (_rtc == nullptr || index >= _alarms.size()) {
        return false;
    }
    
    const AlarmEntry& alarm = _alarms[index];
    
    // Clear existing alarm first
    if (!alarm.enabled) {
        _rtc->disableAlarm();
        return true;
    }
    
    // Set alarm time
    _rtc->setAlarmByMinutes(alarm.minute);
    _rtc->setAlarmByHours(alarm.hour);
    
    // Enable alarm
    _rtc->enableAlarm();
    
    return true;
#endif
}

bool AlarmClock::_clearPCF8563Alarm() {
#ifdef NATIVE_BUILD
    return true;
#else
    if (_rtc != nullptr) {
        _rtc->disableAlarm();
    }
    return true;
#endif
}

void AlarmClock::_handleAlarmFired(uint8_t index) {
    // Only trigger once per alarm occurrence
    if (_hasPendingAlarm && _pendingAlarmIndex == index) {
        return;
    }
    
    _pendingAlarmIndex = index;
    _hasPendingAlarm = true;
    
    if (_alarmCallback) {
        _alarmCallback(index, _alarms[index]);
    }
}

uint8_t AlarmClock::_findFirstEnabledAlarm(uint8_t currentHour, uint8_t currentMinute) {
    for (size_t i = 0; i < _alarms.size(); i++) {
        if (_alarms[i].enabled && checkAlarm(i, currentHour, currentMinute)) {
            return static_cast<uint8_t>(i);
        }
    }
    return 255;
}
