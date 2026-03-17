#include "StepCounter.h"

#ifdef NATIVE_BUILD
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <random>
#else
#include "TTGO.h"
#include <BMA423.h>
#include <TWatch.h>
#endif

StepCounter::StepCounter() 
    : _config{}
    , _status{}
    , _lastUpdateTime(0)
    , _stepsThisUpdate(0)
    , _simulatedStepCount(0)
    , _initialized(false)
    , _progressRingWidget(nullptr)
    , _stepCallback(nullptr)
    , _goalReachedCallback(nullptr)
{
    // Initialize with default config
    _config.dailyGoal = 10000;
    _config.enableNotifications = true;
    _config.enableAutoReset = true;
    
#ifdef NATIVE_BUILD
    // Initialize simulated step count with a small random value to ensure accumulation
    _simulatedStepCount = rand() % 5;
#endif
}

StepCounter::~StepCounter() {
    deinit();
}

bool StepCounter::init() {
#ifdef NATIVE_BUILD
    // Simulate initialization for native builds
    _status.initialized = true;
    _status.lastResetTime = 0;
    _lastUpdateTime = 0;
    _initialized = true;
    return true;
#else
    // Initialize BMA423 accelerometer
    if (!BMA423.begin()) {
        return false;
    }

    // Configure step counter
    if (!BMA423.step_counter_init()) {
        return false;
    }

    // Configure step counter parameters
    BMA423.step_counter_set_threshold(20);  // Step threshold
    BMA423.step_counter_set_time_window(1000);  // 1 second window

    _status.initialized = true;
    _status.lastResetTime = 0;
    _lastUpdateTime = 0;
    _initialized = true;
    return true;
#endif
}

void StepCounter::deinit() {
#ifdef NATIVE_BUILD
    _initialized = false;
    _status.initialized = false;
#else
    if (_initialized) {
        BMA423.step_counter_disable();
        BMA423.end();
        _initialized = false;
        _status.initialized = false;
    }
#endif
}

void StepCounter::setConfig(const Config& config) {
    _config = config;
    _status.goalReached = false;
    _checkGoalReached();
}

const StepCounter::Config& StepCounter::getConfig() const {
    return _config;
}

void StepCounter::setDailyGoal(uint32_t goal) {
    _config.dailyGoal = goal;
    _status.goalReached = false;
    _checkGoalReached();
}

uint32_t StepCounter::getDailyGoal() const {
    return _config.dailyGoal;
}

uint32_t StepCounter::_getStepCountFromSensor() {
#ifdef NATIVE_BUILD
    // Simulate step count for native builds
    // In real scenario, this would call BMA423.step_counter_get_steps()
    // Accumulate steps in a member variable to persist across calls
    _simulatedStepCount += (rand() % 3) + 1;  // Add 1-3 steps per update
    return _simulatedStepCount;
#else
    return BMA423.step_counter_get_steps();
#endif
}

void StepCounter::update() {
    if (!_initialized) {
        return;
    }

#ifdef NATIVE_BUILD
    // Simulate time progression for native builds
    _lastUpdateTime += 100;  // 100ms simulation
#else
    uint32_t currentTime = millis();
    if (_lastUpdateTime == 0) {
        _lastUpdateTime = currentTime;
        return;
    }

    uint32_t elapsed = currentTime - _lastUpdateTime;
    if (elapsed < 100) {
        return;  // Update only every 100ms
    }
    _lastUpdateTime = currentTime;
#endif

    // Get current step count from sensor
    uint32_t currentSteps = _getStepCountFromSensor();
    
    // Calculate new steps since last update (handle wrap-around)
    uint32_t newSteps = 0;
    if (currentSteps >= _status.stepsSinceMidnight) {
        newSteps = currentSteps - _status.stepsSinceMidnight;
    } else {
        // Counter wrap-around detected, use current value
        newSteps = currentSteps;
    }
    
    _stepsThisUpdate = newSteps;
    
    // Update status
    _status.stepsSinceMidnight = currentSteps;
    _status.totalSteps = currentSteps;
    
    // Check for goal reached
    _checkGoalReached();
    
    // Handle midnight reset if enabled
    _handleMidnightReset();
    
    // Update progress ring if widget is set
    if (_progressRingWidget != nullptr) {
        _updateProgressRingInternal();
    }
}

void StepCounter::_checkGoalReached() {
    if (_status.stepsSinceMidnight >= _config.dailyGoal) {
        if (!_status.goalReached && _config.enableNotifications) {
            _status.goalReached = true;
            if (_goalReachedCallback) {
                _goalReachedCallback();
            }
        }
    } else {
        _status.goalReached = false;
    }
}

void StepCounter::_handleMidnightReset() {
#ifdef NATIVE_BUILD
    // For native builds, simulate midnight every 1000 updates
    if (_config.enableAutoReset && _lastUpdateTime > 100000) {
        resetDailyCount();
        _lastUpdateTime = 0;
    }
#else
    if (!_config.enableAutoReset) {
        return;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 1000)) {
        return;  // Skip reset if time not available
    }

    // Check if it's midnight (hour 0, minute 0)
    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
        // Only reset once per day
        if (_status.lastResetTime != timeinfo.tm_mday) {
            resetDailyCount();
            _status.lastResetTime = timeinfo.tm_mday;
        }
    }
#endif
}

uint32_t StepCounter::getStepCount() const {
    return _status.stepsSinceMidnight;
}

uint32_t StepCounter::getStepsSinceMidnight() const {
    return _status.stepsSinceMidnight;
}

StepCounter::Status StepCounter::getStatus() const {
    return _status;
}

void StepCounter::setProgressRingWidget(void* ringWidget) {
    _progressRingWidget = ringWidget;
}

void StepCounter::updateProgressRing() {
    if (_progressRingWidget != nullptr) {
        _updateProgressRingInternal();
    }
}

void StepCounter::_updateProgressRingInternal() {
#ifdef NATIVE_BUILD
    // For native builds, just mark as updated
    (void)_progressRingWidget;
#else
    // Update LVGL progress ring
    if (_config.dailyGoal > 0) {
        float progress = static_cast<float>(_status.stepsSinceMidnight) / _config.dailyGoal;
        if (progress > 1.0f) {
            progress = 1.0f;
        }
        
        // Assuming ringWidget is an lv_obj_t* for progress indicator
        lv_obj_set_style_val(_progressRingWidget, progress * 100, 0);
    }
#endif
}

void StepCounter::setStepCallback(StepCallback callback) {
    _stepCallback = callback;
}

void StepCounter::setGoalReachedCallback(GoalReachedCallback callback) {
    _goalReachedCallback = callback;
}

void StepCounter::resetDailyCount() {
    _status.stepsSinceMidnight = 0;
    _status.goalReached = false;
    _status.lastResetTime = 0;
#ifdef NATIVE_BUILD
    _lastUpdateTime = 0;
#else
    _lastUpdateTime = millis();
#endif
}

void StepCounter::resetAllCounters() {
    resetDailyCount();
    _status.totalSteps = 0;
}

bool StepCounter::isGoalReached() const {
    return _status.goalReached;
}
