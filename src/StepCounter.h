#ifndef STEP_COUNTER_H
#define STEP_COUNTER_H

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>
#else
#include <Arduino.h>
#endif

// Forward declaration for LVGL integration
class TWatch;

class StepCounter {
public:
    // Step counter configuration
    struct Config {
        uint32_t dailyGoal = 10000;  // Default 10k steps goal
        bool enableNotifications = true;
        bool enableAutoReset = true;  // Auto-reset at midnight
    };

    // Step counter status
    struct Status {
        uint32_t dailySteps = 0;
        uint32_t totalSteps = 0;
        uint32_t stepsSinceMidnight = 0;
        uint32_t lastResetTime = 0;
        bool goalReached = false;
        bool initialized = false;
    };

    // Callback type for step events
    using StepCallback = std::function<void(uint32_t steps)>;
    using GoalReachedCallback = std::function<void()>;

    StepCounter();
    ~StepCounter();

    // Initialization
    bool init();
    void deinit();

    // Configuration
    void setConfig(const Config& config);
    const Config& getConfig() const;
    void setDailyGoal(uint32_t goal);
    uint32_t getDailyGoal() const;

    // Step counting
    void update();  // Should be called periodically (e.g., every 100ms)
    uint32_t getStepCount() const;
    uint32_t getStepsSinceMidnight() const;
    Status getStatus() const;

    // LVGL integration
    void setProgressRingWidget(void* ringWidget);  // LVGL ring widget handle
    void updateProgressRing();  // Update the ring based on current steps

    // Notifications
    void setStepCallback(StepCallback callback);
    void setGoalReachedCallback(GoalReachedCallback callback);

    // Manual control
    void resetDailyCount();
    void resetAllCounters();
    bool isGoalReached() const;

private:
    // Internal state
    Config _config;
    Status _status;
    uint32_t _lastUpdateTime;
    uint32_t _stepsThisUpdate;
    uint32_t _simulatedStepCount;  // For native builds simulation
    bool _initialized;

    // LVGL integration
    void* _progressRingWidget;

    // Callbacks
    StepCallback _stepCallback;
    GoalReachedCallback _goalReachedCallback;

    // Helper functions
    void _checkGoalReached();
    void _handleMidnightReset();
    uint32_t _getStepCountFromSensor();
    void _updateProgressRingInternal();
};

#endif // STEP_COUNTER_H
