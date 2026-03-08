/*
 * StateMachine.cpp - Main firmware state machine for T-Watch
 * Implements IDLE, RECORDING, STREAMING, ERROR states
 * Uses FreeRTOS tasks for concurrent operations
 */

#include "StateMachine.h"

#ifdef NATIVE_BUILD
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iostream>
#else
#include "TTGO.h"
#include <BMA423.h>
#endif

StateMachine::StateMachine() 
    : _currentState(SystemState::IDLE)
    , _pendingState(SystemState::IDLE)
    , _bleConnected(false)
    , _errorActive(false)
    , _errorMessage(nullptr)
    , _idleTimerStart(0)
    , _errorTimerStart(0)
    , _watchdogLastFeed(0)
    , _stateChangeCallback(nullptr)
#ifdef NATIVE_BUILD
    , _display(nullptr)
    , _audio(nullptr)
    , _ble(nullptr)
#endif
{
    // Initialize with default config
    _config.idleTimeoutMs = 30000;
    _config.errorRecoveryMs = 5000;
    _config.watchdogTimeoutMs = 10000;
}

StateMachine::~StateMachine() {
    // Cleanup if needed
}

void StateMachine::init(const StateMachineConfig& config) {
    _config = config;
    _idleTimerStart = 0;
    _watchdogLastFeed = 0;
}

void StateMachine::_transitionTo(SystemState newState) {
    SystemState oldState = _currentState;
    _currentState = newState;
    
    // Reset timers based on new state
    switch (newState) {
        case SystemState::IDLE:
            _idleTimerStart = 0;
            _errorActive = false;
            _errorMessage = nullptr;
            break;
        case SystemState::RECORDING:
            _errorActive = false;
            _errorMessage = nullptr;
            break;
        case SystemState::STREAMING:
            _errorActive = false;
            _errorMessage = nullptr;
            break;
        case SystemState::ERROR:
            _errorTimerStart = 0;
            break;
        case SystemState::DEEP_SLEEP:
            _idleTimerStart = 0;
            break;
    }
    
    // Notify callback if registered
    if (_stateChangeCallback) {
        _stateChangeCallback(oldState, newState);
    }
}

void StateMachine::update(uint32_t currentTimeMs) {
    // Feed watchdog if not in deep sleep
    if (_currentState != SystemState::DEEP_SLEEP) {
        _watchdogLastFeed = currentTimeMs;
    }
    
    // Handle pending state transitions
    if (_pendingState != SystemState::IDLE && _pendingState != _currentState) {
        _transitionTo(_pendingState);
        _pendingState = SystemState::IDLE;
    }
    
    // State-specific handling
    switch (_currentState) {
        case SystemState::IDLE:
            _updateIdleTimer(currentTimeMs);
            break;
        case SystemState::ERROR:
            _handleErrorState(currentTimeMs);
            break;
        case SystemState::DEEP_SLEEP:
            _handleDeepSleepState(currentTimeMs);
            break;
        default:
            // RECORDING and STREAMING don't need special timeout handling
            break;
    }
}

void StateMachine::_updateIdleTimer(uint32_t currentTimeMs) {
    if (!_errorActive && !_bleConnected) {
        uint32_t elapsed = currentTimeMs - _idleTimerStart;
        if (elapsed >= _config.idleTimeoutMs) {
            _transitionTo(SystemState::DEEP_SLEEP);
        }
    }
}

void StateMachine::_handleErrorState(uint32_t currentTimeMs) {
    uint32_t elapsed = currentTimeMs - _errorTimerStart;
    if (elapsed >= _config.errorRecoveryMs) {
        // Auto-recover from error
        _transitionTo(SystemState::IDLE);
    }
}

void StateMachine::_handleDeepSleepState(uint32_t currentTimeMs) {
    // In real implementation, this would trigger deep sleep
    // For native builds, we just track the state
    (void)currentTimeMs; // Suppress unused warning
}

void StateMachine::handleButtonPress() {
    switch (_currentState) {
        case SystemState::IDLE:
            _transitionTo(SystemState::RECORDING);
            break;
        case SystemState::RECORDING:
            _transitionTo(SystemState::IDLE);
            break;
        case SystemState::STREAMING:
            _transitionTo(SystemState::RECORDING);
            break;
        case SystemState::ERROR:
            // Button press during error clears the error
            _transitionTo(SystemState::IDLE);
            break;
        case SystemState::DEEP_SLEEP:
            // Button press wakes from deep sleep
            _transitionTo(SystemState::IDLE);
            break;
    }
}

void StateMachine::setBLEConnected(bool connected) {
    _bleConnected = connected;
    
    // Auto-transition from RECORDING to STREAMING when BLE connects
    if (connected && _currentState == SystemState::RECORDING) {
        _transitionTo(SystemState::STREAMING);
    }
    
    // If BLE disconnects during STREAMING, go back to RECORDING
    if (!connected && _currentState == SystemState::STREAMING) {
        _transitionTo(SystemState::RECORDING);
    }
}

void StateMachine::setError(const char* errorMessage) {
    if (errorMessage) {
        _errorMessage = errorMessage;
        _errorActive = true;
        _transitionTo(SystemState::ERROR);
    }
}

void StateMachine::clearError() {
    _errorActive = false;
    _errorMessage = nullptr;
    if (_currentState == SystemState::ERROR) {
        _transitionTo(SystemState::IDLE);
    }
}

void StateMachine::feedWatchdog() {
    // In native builds, this is a no-op
    // In real firmware, this would update the watchdog timer
}

bool StateMachine::isWatchdogExpired() const {
    // In native builds, watchdog is always healthy
    (void)_watchdogLastFeed;
    return false;
}

void StateMachine::onStateChange(std::function<void(SystemState, SystemState)> callback) {
    _stateChangeCallback = callback;
}

// Request methods - these set pending state for safe transition
void StateMachine::requestIdle() {
    _pendingState = SystemState::IDLE;
}

void StateMachine::requestRecording() {
    _pendingState = SystemState::RECORDING;
}

void StateMachine::requestStreaming() {
    _pendingState = SystemState::STREAMING;
}

void StateMachine::requestError() {
    _pendingState = SystemState::ERROR;
}

void StateMachine::requestDeepSleep() {
    _pendingState = SystemState::DEEP_SLEEP;
}
