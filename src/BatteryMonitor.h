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
        bool initialized = false;
    };

    using BatteryCallback = std::function<void(const Status&)>;
    using AlertCallback = std::function<void(const char* alertType, int percentage)>;

    BatteryMonitor();
    ~BatteryMonitor();

    bool init();
    void deinit();
    void update();

    Status getStatus() const;
    int getPercentage() const;
    float getVoltage() const;
    bool isCharging() const;

    void setConfig(const Config& config);
    Config getConfig() const;

    void setAlertCallback(AlertCallback callback);
    void enablePowerSaving(bool enable);
    void setLVGLWidget(void* widget);

private:
    Config _config;
    Status _status;
    bool _initialized;
    AlertCallback _alertCallback;
    void* _lvglWidget;

    // Simulation state for native builds
    uint32_t _updateCount;
    bool _prevLowBattery;
    bool _prevCritical;

    void _checkAlerts();
    void _updatePowerSaving();
    void _readBatteryData();
    void _updateLVGLWidget();
};

#endif // BATTERY_MONITOR_H
