#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>
#else
#include <Arduino.h>
#endif

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
        float dischargeCurrent = 0.0f;
        bool isLowBattery = false;
        bool isCritical = false;
        bool powerSavingActive = false;
    };

    using AlertCallback = std::function<void(const char* alertType, int percentage)>;

    BatteryMonitor();
    ~BatteryMonitor();

    bool init();
    void deinit();
    void update();

    // Getters
    int getPercentage() const;
    float getVoltage() const;
    bool isCharging() const;
    bool isUSBConnected() const;
    float getTemperature() const;
    Status getStatus() const;

    // Config
    void setConfig(const Config& config);
    const Config& getConfig() const;

    // Alerts
    void setAlertCallback(AlertCallback callback);
    bool isLowBattery() const;
    bool isCritical() const;

    // Power saving
    void enablePowerSaving(bool enable);
    bool isPowerSavingActive() const;

    // LVGL widget
    void setBatteryWidget(void* widget);

private:
    Config _config;
    Status _status;
    bool _initialized;
    AlertCallback _alertCallback;
    void* _batteryWidget;
    bool _lastLowAlert;
    bool _lastCriticalAlert;
    uint32_t _updateCount;

    void _readBatteryData();
    void _checkAlerts();
    void _updatePowerSaving();
};

#endif // BATTERY_MONITOR_H
