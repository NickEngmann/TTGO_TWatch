#include "StateMachineROSWrapper.h"
#include "StateMachine.cpp"

StateMachineROSWrapper::StateMachineROSWrapper() 
    : _currentROSState(ROS_IDLE)
    , _lastValidROSState(ROS_IDLE)
    , _transitionInProgress(false)
    , _lastConnectionHandle(0) {
}

StateMachineROSWrapper::~StateMachineROSWrapper() {
    // Cleanup if needed
}

bool StateMachineROSWrapper::init() {
    if (!_legacySM.init()) {
        return false;
    }
    
    _currentROSState = ROS_IDLE;
    _lastValidROSState = ROS_IDLE;
    _transitionInProgress = false;
    
    return true;
}

void StateMachineROSWrapper::update(uint32_t currentTimeMs) {
    _legacySM.update(currentTimeMs);
    _syncROSStateWithLegacy();
}

StateMachine::State StateMachineROSWrapper::toLegacyState(ROSState rosState) {
    switch (rosState) {
        case ROS_IDLE:
            return StateMachine::IDLE;
        case ROS_CONNECTING:
        case ROS_CONNECTED:
        case ROS_STREAMING:
        case ROS_PAUSED:
        case ROS_ERROR_RECOVERING:
        case ROS_ERROR:
        case ROS_DEEP_SLEEP:
        default:
            return StateMachine::IDLE;
    }
}

StateMachineROSWrapper::ROSState StateMachineROSWrapper::toROSState(StateMachine::State legacyState) {
    switch (legacyState) {
        case StateMachine::IDLE:
            return ROS_IDLE;
        case StateMachine::RECORDING:
            return ROS_STREAMING;
        case StateMachine::STREAMING:
            return ROS_STREAMING;
        case StateMachine::ERROR:
            return ROS_ERROR;
        case StateMachine::DEEP_SLEEP:
            return ROS_DEEP_SLEEP;
        default:
            return ROS_IDLE;
    }
}

void StateMachineROSWrapper::_syncROSStateWithLegacy() {
    StateMachine::State legacyState = _legacySM.getCurrentState();
    ROSState newROSState = toROSState(legacyState);
    
    if (_currentROSState != newROSState) {
        _currentROSState = newROSState;
        _handleLegacyStateChange(legacyState, legacyState);
    }
}

void StateMachineROSWrapper::_handleLegacyStateChange(StateMachine::State oldState,
                                                       StateMachine::State newState) {
    (void)oldState;
    (void)newState;
    // Could add logging here
}

StateMachineROSWrapper::ROSState StateMachineROSWrapper::getCurrentROSState() const {
    return _currentROSState;
}

const char* StateMachineROSWrapper::getROSStateName() const {
    switch (_currentROSState) {
        case ROS_IDLE: return "IDLE";
        case ROS_CONNECTING: return "CONNECTING";
        case ROS_CONNECTED: return "CONNECTED";
        case ROS_STREAMING: return "STREAMING";
        case ROS_PAUSED: return "PAUSED";
        case ROS_ERROR_RECOVERING: return "ERROR_RECOVERING";
        case ROS_ERROR: return "ERROR";
        case ROS_DEEP_SLEEP: return "DEEP_SLEEP";
        default: return "UNKNOWN";
    }
}

uint32_t StateMachineROSWrapper::getTimeInCurrentROSState(uint32_t currentTimeMs) const {
    return _legacySM.getTimeInState(currentTimeMs);
}

void StateMachineROSWrapper::setROSConnectCallback(ROSShutdownCallback callback) {
    (void)callback;
    // Implementation would connect ROS callbacks
}

void StateMachineROSWrapper::setROSDisconnectCallback(ROSDisconnectCallback callback) {
    (void)callback;
    // Implementation would handle ROS disconnects
}

void StateMachineROSWrapper::onButtonPress(uint32_t currentTimeMs) {
    _legacySM.onButtonPress();
    _syncROSStateWithLegacy();
    
    if (_currentROSState == ROS_CONNECTING) {
        _lastConnectionHandle = 0;
    }
}

void StateMachineROSWrapper::onBLEConnect(uint32_t currentTimeMs, uint16_t handle) {
    _legacySM.onBLEConnect();
    _syncROSStateWithLegacy();
    
    if (_currentROSState == ROS_CONNECTED || _currentROSState == ROS_CONNECTING) {
        _lastConnectionHandle = handle;
        if (_connectCallback) {
            _connectCallback(handle);
        }
    }
}

void StateMachineROSWrapper::onBLEDisconnect(uint32_t currentTimeMs, uint16_t handle) {
    (void)currentTimeMs;
    (void)handle;
    
    _legacySM.onBLEDisconnect();
    _syncROSStateWithLegacy();
    
    if (_disconnectCallback && _lastConnectionHandle != 0) {
        _disconnectCallback(_lastConnectionHandle);
        _lastConnectionHandle = 0;
    }
}

void StateMachineROSWrapper::onError(const char* errorReason, uint32_t currentTimeMs) {
    (void)errorReason;
    (void)currentTimeMs;
    
    _legacySM.onError();
    _syncROSStateWithLegacy();
    
    if (_shutdownCallback) {
        _shutdownCallback(errorReason);
    }
}

void StateMachineROSWrapper::onWatchdogTimeout(uint32_t currentTimeMs) {
    (void)currentTimeMs;
    
    _legacySM.onButtonPress();  // Simulate activity
    _syncROSStateWithLegacy();
}

bool StateMachineROSWrapper::isTransitionValid(ROSState from, ROSState to) const {
    switch (from) {
        case ROS_IDLE:
            return to == ROS_CONNECTING || to == ROS_DEEP_SLEEP;
        case ROS_CONNECTING:
            return to == ROS_CONNECTED || to == ROS_ERROR_RECOVERING || to == ROS_IDLE;
        case ROS_CONNECTED:
            return to == ROS_STREAMING || to == ROS_PAUSED || to == ROS_IDLE || 
                   to == ROS_ERROR_RECOVERING;
        case ROS_STREAMING:
            return to == ROS_PAUSED || to == ROS_IDLE || to == ROS_ERROR_RECOVERING;
        case ROS_PAUSED:
            return to == ROS_STREAMING || to == ROS_IDLE || to == ROS_ERROR_RECOVERING;
        case ROS_ERROR_RECOVERING:
            return to == ROS_CONNECTED || to == ROS_ERROR || to == ROS_IDLE;
        case ROS_ERROR:
            return to == ROS_IDLE || to == ROS_DEEP_SLEEP;
        case ROS_DEEP_SLEEP:
            return to == ROS_IDLE;
        default:
            return false;
    }
}

bool StateMachineROSWrapper::isValidConnectionState() const {
    return _currentROSState == ROS_CONNECTED || 
           _currentROSState == ROS_STREAMING ||
           _currentROSState == ROS_PAUSED;
}

bool StateMachineROSWrapper::shouldAttemptRecovery() const {
    return _currentROSState == ROS_ERROR ||
           _currentROSState == ROS_ERROR_RECOVERING;
}
