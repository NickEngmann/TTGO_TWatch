#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>
#else
#include <Arduino.h>
#endif

// Forward declarations for hardware integration
class TWatchDisplay;
class AudioRecorder;
class BLEManager;

enum class SystemState : uint8_t {
    IDLE = 0,
    RECORDING = 1,
    STREAMING = 2,
    ERROR = 3,
    DEEP_SLEEP = 4
};

struct StateMachineConfig {
    uint32_t idleTimeoutMs = 30000;  // 30 seconds deep sleep timeout
    uint32_t errorRecoveryMs = 5000; // 5 seconds auto-recovery
    uint32_t watchdogTimeoutMs = 10000; // 10 seconds watchdog
};

class StateMachine {
public:
    StateMachine();
    ~StateMachine();

    // Initialize with hardware dependencies (only in non-native builds)
#ifdef NATIVE_BUILD
    void init(const StateMachineConfig& config = StateMachineConfig());
#else
    void init(TWatchDisplay* display, AudioRecorder* audio, BLEManager* ble,
              const StateMachineConfig& config = StateMachineConfig());
#endif

    // State management
    SystemState getCurrentState() const { return _currentState; }
    bool isRecording() const { return _currentState == SystemState::RECORDING; }
    bool isStreaming() const { return _currentState == SystemState::STREAMING; }
    bool isError() const { return _currentState == SystemState::ERROR; }
    
    // State transitions
    void requestIdle();
    void requestRecording();
    void requestStreaming();
    void requestError();
    void requestDeepSleep();
    
    // Main loop update (called periodically)
    void update(uint32_t currentTimeMs);
    
    // Button press handler
    void handleButtonPress();
    
    // BLE connection status
    void setBLEConnected(bool connected);
    bool isBLEConnected() const { return _bleConnected; }
    
    // Error handling
    void setError(const char* errorMessage);
    void clearError();
    
    // Watchdog management
    void feedWatchdog();
    bool isWatchdogExpired() const;
    
    // State change callback
    void onStateChange(std::function<void(SystemState, SystemState)> callback);

private:
    SystemState _currentState;
    SystemState _pendingState;
    bool _bleConnected;
    bool _errorActive;
    const char* _errorMessage;
    
    // Timers
    uint32_t _idleTimerStart;
    uint32_t _errorTimerStart;
    uint32_t _watchdogLastFeed;
    
    // Configuration
    StateMachineConfig _config;
    
    // Callbacks
    std::function<void(SystemState, SystemState)> _stateChangeCallback;
    
    // Hardware references (only in non-native builds)
#ifdef NATIVE_BUILD
    TWatchDisplay* _display = nullptr;
    AudioRecorder* _audio = nullptr;
    BLEManager* _ble = nullptr;
#else
    TWatchDisplay* _display;
    AudioRecorder* _audio;
    BLEManager* _ble;
#endif

    // Internal transition helper
    void _transitionTo(SystemState newState);
    void _updateIdleTimer(uint32_t currentTimeMs);
    void _handleErrorState(uint32_t currentTimeMs);
    void _handleDeepSleepState(uint32_t currentTimeMs);
};

#endif // STATE_MACHINE_H
