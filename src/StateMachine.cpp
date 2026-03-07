/*
 * StateMachine.cpp - Main firmware state machine for T-Watch
 * 
 * States: IDLE, RECORDING, STREAMING, ERROR
 * Handles: Button press, BLE connection, audio recording, power management
 */

#include "StateMachine.h"
#include "TTGO.h"

WatchStateMachine::WatchStateMachine() {
    currentState = STATE_IDLE;
    previousState = STATE_IDLE;
    bleStatus = BLE_DISCONNECTED;
    errorState = ERROR_NONE;
    lastButtonPressTime = 0;
    lastActivityTime = 0;
    errorStartTime = 0;
    idleTimeoutMs = 30000;  // 30 seconds
    errorRecoveryTimeoutMs = 5000;  // 5 seconds
    lowPowerMode = false;
    stateCallback = nullptr;
    buttonCallback = nullptr;
}

WatchStateMachine::~WatchStateMachine() {
    // Cleanup if needed
}

void WatchStateMachine::update(uint32_t currentTime) {
    // Update activity time
    if (currentState != STATE_ERROR) {
        lastActivityTime = currentTime;
    }
    
    // Handle state-specific logic
    switch (currentState) {
        case STATE_IDLE:
            handleIdleState(currentTime);
            break;
        case STATE_RECORDING:
            handleRecordingState(currentTime);
            break;
        case STATE_STREAMING:
            handleStreamingState(currentTime);
            break;
        case STATE_ERROR:
            handleErrorState(currentTime);
            break;
    }
    
    // Check for button events
    checkButtonEvents(currentTime);
    
    // Check idle timeout
    checkIdleTimeout(currentTime);
    
    // Check error recovery
    checkErrorRecovery(currentTime);
}

void WatchStateMachine::handleIdleState(uint32_t currentTime) {
    // In idle state, wait for button press or BLE connection
    // If BLE connects, transition to recording
    if (bleStatus == BLE_CONNECTED) {
        transitionTo(STATE_RECORDING);
    }
}

void WatchStateMachine::handleRecordingState(uint32_t currentTime) {
    // In recording state, capture audio and monitor BLE connection
    // If BLE connects, transition to streaming
    if (bleStatus == BLE_CONNECTED) {
        transitionTo(STATE_STREAMING);
    }
}

void WatchStateMachine::handleStreamingState(uint32_t currentTime) {
    // In streaming state, send audio over BLE
    // Monitor BLE connection status
    if (bleStatus == BLE_DISCONNECTED) {
        // BLE disconnected during streaming, go back to recording
        transitionTo(STATE_RECORDING);
    }
}

void WatchStateMachine::handleErrorState(uint32_t currentTime) {
    // In error state, monitor for recovery conditions
    // Error recovery is handled in checkErrorRecovery()
}

void WatchStateMachine::checkButtonEvents(uint32_t currentTime) {
    // Check for button press events
    // This would interface with TTGO.cpp in real implementation
    // For native build, we simulate button events via setButtonState()
    
    if (buttonState == BUTTON_PRESS) {
        // Button was pressed
        switch (currentState) {
            case STATE_IDLE:
                // Start recording
                transitionTo(STATE_RECORDING);
                break;
            case STATE_RECORDING:
                // Stop recording
                transitionTo(STATE_IDLE);
                break;
            case STATE_STREAMING:
                // Stop streaming
                transitionTo(STATE_IDLE);
                break;
            case STATE_ERROR:
                // Reset error state
                clearError();
                transitionTo(STATE_IDLE);
                break;
        }
        
        // Call button callback if registered
        if (buttonCallback) {
            buttonCallback(currentState);
        }
        
        buttonState = BUTTON_RELEASED;
    }
}

void WatchStateMachine::checkIdleTimeout(uint32_t currentTime) {
    // Check if we've been idle for too long without BLE connection
    if (bleStatus != BLE_CONNECTED) {
        uint32_t idleTime = currentTime - lastActivityTime;
        
        if (idleTime >= idleTimeoutMs) {
            // Enter deep sleep mode
            if (lowPowerMode) {
                // In real implementation, this would trigger deep sleep
                // For native build, just log the action
            }
        }
    }
}

void WatchStateMachine::checkErrorRecovery(uint32_t currentTime) {
    // Check if error recovery timeout has elapsed
    uint32_t errorDuration = currentTime - errorStartTime;
    
    if (errorDuration >= errorRecoveryTimeoutMs) {
        // Auto-recover from error
        clearError();
        transitionTo(STATE_IDLE);
    }
}

void WatchStateMachine::updateActivityTime(uint32_t currentTime) {
    // Update activity time based on current state
    // Only update if we're not in error state
    if (currentState != STATE_ERROR) {
        // Activity time is updated on state transitions
        // This is handled in transitionTo()
    }
}

void WatchStateMachine::transitionTo(WatchState newState) {
    // Handle state transition
    previousState = currentState;
    currentState = newState;
    
    // Update activity time on state change
    lastActivityTime = millis();
    
    // Call state change callback if registered
    if (stateCallback) {
        stateCallback(newState);
    }
    
    // Log state transition for debugging
    Serial.printf("State transition: %d -> %d\n", previousState, newState);
}

void WatchStateMachine::setBLEStatus(BLEStatus status) {
    bleStatus = status;
    
    // If BLE connects and we're in idle state, start recording
    if (status == BLE_CONNECTED && currentState == STATE_IDLE) {
        transitionTo(STATE_RECORDING);
    }
}

void WatchStateMachine::setError(ErrorCode error) {
    errorState = error;
    errorStartTime = millis();
    transitionTo(STATE_ERROR);
}

void WatchStateMachine::clearError() {
    errorState = ERROR_NONE;
    errorStartTime = 0;
}

void WatchStateMachine::setLowPowerMode(bool mode) {
    lowPowerMode = mode;
}

void WatchStateMachine::setButtonState(ButtonState state) {
    buttonState = state;
}

void WatchStateMachine::setStateCallback(StateCallback callback) {
    stateCallback = callback;
}

void WatchStateMachine::setButtonCallback(ButtonCallback callback) {
    buttonCallback = callback;
}

WatchState WatchStateMachine::getCurrentState() const {
    return currentState;
}

WatchState WatchStateMachine::getPreviousState() const {
    return previousState;
}

BLEStatus WatchStateMachine::getBLEStatus() const {
    return bleStatus;
}

ErrorCode WatchStateMachine::getErrorState() const {
    return errorState;
}

bool WatchStateMachine::isBLEConnected() const {
    return bleStatus == BLE_CONNECTED;
}

bool WatchStateMachine::isInErrorState() const {
    return currentState == STATE_ERROR;
}

bool WatchStateMachine::isLowPowerMode() const {
    return lowPowerMode;
}

uint32_t WatchStateMachine::getIdleTimeout() const {
    return idleTimeoutMs;
}

uint32_t WatchStateMachine::getErrorRecoveryTimeout() const {
    return errorRecoveryTimeoutMs;
}

void WatchStateMachine::setIdleTimeout(uint32_t timeoutMs) {
    idleTimeoutMs = timeoutMs;
}

void WatchStateMachine::setErrorRecoveryTimeout(uint32_t timeoutMs) {
    errorRecoveryTimeoutMs = timeoutMs;
}
