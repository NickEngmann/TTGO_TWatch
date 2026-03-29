#ifndef STATE_MACHINE_WRAPPER_ROS_H
#define STATE_MACHINE_WRAPPER_ROS_H

#include "StateMachine.h"

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>
#else
#include <Arduino.h>
#endif

// Wrapper for existing StateMachine to provide ROS integration
class StateMachineROSWrapper {
public:
    enum ROSState {
        ROS_IDLE = 0,
        ROS_CONNECTING,
        ROS_CONNECTED,
        ROS_STREAMING,
        ROS_PAUSED,
        ROS_ERROR_RECOVERING,
        ROS_ERROR,
        ROS_DEEP_SLEEP,
        ROS_COUNT
    };

    using ROSShutdownCallback = std::function<void(const char* reason)>;
    using ROSConnectCallback = std::function<void(uint16_t connectionHandle)>;
    using ROSDisconnectCallback = std::function<void(uint16_t connectionHandle)>;

    StateMachineROSWrapper();
    ~StateMachineROSWrapper();

    // Initialization
    bool init();
    void update(uint32_t currentTimeMs);
    
    // Convert between old and new state enums
    static StateMachine::State toLegacyState(ROSState rosState);
    static ROSState toROSState(StateMachine::State legacyState);
    
    // Event handlers
    void onButtonPress(uint32_t currentTimeMs);
    void onBLEConnect(uint32_t currentTimeMs, uint16_t handle);
    void onBLEDisconnect(uint32_t currentTimeMs, uint16_t handle);
    void onError(const char* errorReason, uint32_t currentTimeMs);
    void onWatchdogTimeout(uint32_t currentTimeMs);
    
    // ROS-specific features
    void setROSConnectCallback(ROSShutdownCallback callback);
    void setROSDisconnectCallback(ROSDisconnectCallback callback);
    
    // Query methods
    ROSState getCurrentROSState() const;
    const char* getROSStateName() const;
    uint32_t getTimeInCurrentROSState(uint32_t currentTimeMs) const;
    
    // Transition history
    bool isTransitionValid(ROSState from, ROSState to) const;
    bool isValidConnectionState() const;
    bool shouldAttemptRecovery() const;
    
    // Legacy state machine access
    StateMachine* getLegacyStateMachine() { return &_legacySM; }
    
private:
    StateMachine _legacySM;
    ROSState _currentROSState;
    ROSState _lastValidROSState;
    
    // Callbacks
    ROSShutdownCallback _shutdownCallback;
    ROSConnectCallback _connectCallback;
    ROSDisconnectCallback _disconnectCallback;
    
    // Tracking
    bool _transitionInProgress;
    uint16_t _lastConnectionHandle;
    
    void _syncROSStateWithLegacy();
    void _handleLegacyStateChange(StateMachine::State oldState, 
                                   StateMachine::State newState);
};

#endif // STATE_MACHINE_WRAPPER_ROS_H
