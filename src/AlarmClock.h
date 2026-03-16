#ifndef ALARM_CLOCK_H
#define ALARM_CLOCK_H

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>
#include <vector>
#else
#include <Arduino.h>
#include "../drive/rtc/pcf8563.h"
#endif

// Alarm clock entry
struct AlarmEntry {
    uint8_t hour;
    uint8_t minute;
    bool enabled;
    uint32_t repeatDays;  // Bitmask for repeat: bit0=Mon, bit1=Tue, etc.
    char label[32];
};

// Alarm alarm event type
enum class AlarmEventType {
    NO_ALARM,
    MINUTE_ALARM,
    HOUR_ALARM,
    DAY_ALARM,
    WEEKDAY_ALARM
};

class AlarmClock {
public:
    // Callback types
    using AlarmCallback = std::function<void(uint8_t index, AlarmEntry& alarm)>;

    // Alarm configuration
    struct Config {
        bool autoCheckEnabled = true;
        uint32_t checkIntervalMs = 1000;  // Check interval in milliseconds
        bool playTone = true;
        bool vibrate = true;
    };

    AlarmClock();
    ~AlarmClock();

    // Initialization
    bool init();
    void deinit();
    void update();  // Call this periodically

    // Alarm management
    uint8_t addAlarm(uint8_t hour, uint8_t minute, const char* label = "", uint32_t repeatDays = 0);
    bool removeAlarm(uint8_t index);
    bool enableAlarm(uint8_t index);
    bool disableAlarm(uint8_t index);
    bool toggleAlarm(uint8_t index);
    
    // Alarm configuration
    bool setAlarmTime(uint8_t index, uint8_t hour, uint8_t minute);
    bool setAlarmLabel(uint8_t index, const char* label);
    bool setAlarmRepeat(uint8_t index, uint32_t repeatDays);
    
    // Alarm status
    AlarmEntry getAlarm(uint8_t index) const;
    uint8_t getAlarmCount() const;
    bool isAlarmActive(uint8_t index) const;
    bool checkAlarm(uint8_t index, uint8_t currentHour, uint8_t currentMinute);
    
    // Event callback
    void setAlarmCallback(AlarmCallback callback);
    bool hasPendingAlarm() const;
    uint8_t getPendingAlarmIndex() const;
    void clearPendingAlarm();
    
    // Config
    void setConfig(const Config& config);
    const Config& getConfig() const;
    
    // Low-level PCF8563 access (for direct RTC control)
    bool hasAlarmOccurred();
    AlarmEventType getAlarmType();

private:
    Config _config;
    std::vector<AlarmEntry> _alarms;
    bool _initialized;
    
    // RTC access
#ifdef NATIVE_BUILD
    uint8_t _currentTimeHour;
    uint8_t _currentTimeMinute;
#else
    PCF8563_Class* _rtc;
#endif
    
    // Event tracking
    AlarmCallback _alarmCallback;
    uint8_t _pendingAlarmIndex;
    bool _hasPendingAlarm;
    
    // Internal functions
    bool _setupPCF8563Alarm(uint8_t index);
    bool _clearPCF8563Alarm();
    void _handleAlarmFired(uint8_t index);
    uint8_t _findFirstEnabledAlarm(uint8_t currentHour, uint8_t currentMinute);
};

#endif // ALARM_CLOCK_H
