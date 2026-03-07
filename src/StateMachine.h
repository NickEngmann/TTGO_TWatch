/*
 * StateMachine.h - Main firmware state machine for T-Watch
 * 
 * States: IDLE, RECORDING, STREAMING, ERROR
 * Handles: Button press, BLE connection, audio recording, power management
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

class WatchStateMachine {
public:
    // State constants
    static const int STATE_IDLE = 0;
    static const int STATE_RECORDING = 1;
    static const int STATE_STREAMING = 2;
    static const int STATE_ERROR = 3;
    
    // Error constants
    static const int ERROR_NONE = 0;
    static const int ERROR_AUDIO = 1;
    static const int ERROR_BLE = 2;
    static const int ERROR_POWER = 3;
    
    // Button constants
    static const int BUTTON_PRESS = 1;
    
    // BLE status constants
    static const int BLE_DISCONNECTED = 0;
    static const int BLE_CONNECTED = 1;
    static const int BLE_ERROR = 2;
    
    // Constructor and destructor
    StateMachine();
    ~StateMachine();
    
    // Initialization
    void begin();
    
    // Main update loop
    void run();
    
    // State access
    int getState();
    int getPreviousState();
    bool isStreaming();
    bool isRecording();
    bool isError();
    
    // Button handling
    void handleButtonPress(int buttonEvent);
    void resetButtonEvents();
    
    // BLE handling
    void setBLEConnected(bool connected);
    bool isBLEConnected();
    void setBLEStatus(int status);
    int getBLEStatus();
    
    // Error handling
    void setError(int errorType);
    void clearError();
    bool hasError();
    int getLastError();
    
    // Timing
    void setElapsedTime(uint32_t elapsedMs);
    uint32_t getElapsedTime();
    uint32_t getRecordingDuration();
    uint32_t getStreamingDuration();
    uint32_t getErrorDuration();
    
    // Power management
    void setLowPowerMode(bool enabled);
    void setBatteryLevel(float percentage);
    float getBatteryLevel();
    bool isLowPowerMode();
    
    // Callbacks
    void setStateChangeCallback(void (*callback)(int newState));
    void setErrorCallback(void (*callback)(int error));
    void setBLECallback(void (*callback)(int status));
    
private:
    // Internal state
    int currentState;
    int previousState;
    int bleStatus;
    int lastError;
    bool lowPowerMode;
    float batteryLevel;
    
    // Timing variables
    uint32_t idleTimeoutMs;
    uint32_t errorRecoveryTimeoutMs;
    uint32_t elapsedTime;
    uint32_t recordingStartTime;
    uint32_t streamingStartTime;
    uint32_t errorStartTime;
    uint32_t lastActivityTime;
    
    // Button tracking
    uint32_t buttonPressStartTime;
    int buttonPressCount;
    uint32_t lastButtonPressTime;
    
    // Callbacks
    void (*stateChangeCallback)(int newState);
    void (*errorCallback)(int error);
    void (*bleCallback)(int status);
    
    // Private methods
    void transitionTo(int newState);
    void handleIdleState(uint32_t currentTime);
    void handleRecordingState(uint32_t currentTime);
    void handleStreamingState(uint32_t currentTime);
    void handleErrorState(uint32_t currentTime);
    void checkButtonEvents(uint32_t currentTime);
    void checkIdleTimeout(uint32_t currentTime);
    void checkErrorRecovery(uint32_t currentTime);
    void updateActivityTime(uint32_t currentTime);
};

#endif // STATE_MACHINE_H
