#include "ROSStateMachine.h"
#include <cstring>
#include <climits>

ROSStateMachine::ROSStateMachine()
    : _currentState(ROS_IDLE)
    , _previousState(ROS_IDLE)
    , _stateEntryTime(0)
    , _lastActivityTime(0)
    , _initialized(false)
    , _bleConnected(false)
    , _stateChangeFlag(false)
    , _transitionPendingFlag(false)
    , _transitionHistoryIndex(0)
    , _transitionStatsIndex(0)
    , _lastErrorMessage(nullptr)
    , _lastErrorSeverity(ERROR_NONE)
    , _errorCount(0) {

    // Initialize transition history
    for (uint8_t i = 0; i < MAX_TRANSITION_HISTORY; i++) {
        _transitionHistory[i].from = ROS_IDLE;
        _transitionHistory[i].to = ROS_IDLE;
        _transitionHistory[i].timestamp = 0;
    }

    // Initialize state statistics
    for (uint8_t i = 0; i < MAX_STATE_STATS; i++) {
        _stateStats[i].visitCount = 0;
        _stateStats[i].totalDwellTimeMs = 0;
        _stateStats[i].minDwellTimeMs = UINT32_MAX;
        _stateStats[i].maxDwellTimeMs = 0;
    }

    // Initialize transition statistics
    for (uint8_t i = 0; i < MAX_TRANSITION_STATS; i++) {
        _transitionStats[i].transitionCount = 0;
        _transitionStats[i].successCount = 0;
        _transitionStats[i].rejectedCount = 0;
    }
}

ROSStateMachine::~ROSStateMachine() {
    deinit();
}

bool ROSStateMachine::init() {
    if (_initialized) {
        return false;
    }

    _currentState = ROS_IDLE;
    _previousState = ROS_IDLE;
    _stateEntryTime = millis();
    _lastActivityTime = _stateEntryTime;
    _bleConnected = false;
    _stateChangeFlag = false;
    _transitionPendingFlag = false;
    _lastErrorMessage = nullptr;
    _lastErrorSeverity = ERROR_NONE;
    _errorCount = 0;
    _initialized = true;

    _recordTransition(ROS_IDLE, ROS_IDLE, _stateEntryTime);

    return true;
}

void ROSStateMachine::deinit() {
    if (!_initialized) {
        return;
    }
    
    // Clean up callbacks
    _stateChangeCallback = nullptr;
    _transitionValidator = nullptr;
    _preTransitionCallback = nullptr;
    _postTransitionCallback = nullptr;
    
    _initialized = false;
}

ROSStateMachine::State ROSStateMachine::getCurrentState() const {
    return _currentState;
}

ROSStateMachine::State ROSStateMachine::getPreviousState() const {
    return _previousState;
}

const char* ROSStateMachine::getStateName() const {
    switch (_currentState) {
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

uint32_t ROSStateMachine::getTimeInState(uint32_t currentTimeMs) const {
    if (!_initialized) {
        return 0;
    }
    return (currentTimeMs >= _stateEntryTime) ? 
           (currentTimeMs - _stateEntryTime) : 0;
}

bool ROSStateMachine::isValidTransition(ROSStateMachine::State from, ROSStateMachine::State to) const {
    // All states can transition to themselves
    if (from == to) {
        return true;
    }
    
    // Define valid transitions
    switch (from) {
        case ROS_IDLE:
            return to == ROS_CONNECTING || to == ROS_DEEP_SLEEP;
            
        case ROS_CONNECTING:
            return to == ROS_CONNECTED || to == ROS_ERROR_RECOVERING ||
                   to == ROS_IDLE;
            
        case ROS_CONNECTED:
            return to == ROS_STREAMING || to == ROS_PAUSED || 
                   to == ROS_IDLE || to == ROS_ERROR_RECOVERING;
            
        case ROS_STREAMING:
            return to == ROS_PAUSED || to == ROS_IDLE || 
                   to == ROS_ERROR_RECOVERING;
            
        case ROS_PAUSED:
            return to == ROS_STREAMING || to == ROS_IDLE || 
                   to == ROS_ERROR_RECOVERING;
            
        case ROS_ERROR_RECOVERING:
            return to == ROS_CONNECTED || to == ROS_ERROR ||
                   to == ROS_IDLE;
            
        case ROS_ERROR:
            return to == ROS_IDLE || to == ROS_DEEP_SLEEP;
            
        case ROS_DEEP_SLEEP:
            return to == ROS_IDLE;
            
        default:
            return false;
    }
}

ROSStateMachine::TransitionRule ROSStateMachine::getTransitionRule(ROSStateMachine::State from, ROSStateMachine::State to) const {
    if (from == to) {
        return TransitionRule::ALLOWED;
    }

    switch (from) {
        case ROS_IDLE:
            if (to == ROS_CONNECTING || to == ROS_DEEP_SLEEP) return TransitionRule::ALLOWED;
            return TransitionRule::FORBIDDEN;

        case ROS_CONNECTING:
            if (to == ROS_CONNECTED || to == ROS_IDLE) return TransitionRule::ALLOWED;
            if (to == ROS_ERROR_RECOVERING) return TransitionRule::REQUIRES_CLEANUP;
            return TransitionRule::FORBIDDEN;

        case ROS_CONNECTED:
            if (to == ROS_STREAMING || to == ROS_PAUSED) return TransitionRule::ALLOWED;
            if (to == ROS_IDLE) return TransitionRule::CONDITIONAL;
            if (to == ROS_ERROR_RECOVERING) return TransitionRule::REQUIRES_CLEANUP;
            return TransitionRule::FORBIDDEN;
            
        case ROS_STREAMING:
            if (to == ROS_PAUSED || to == ROS_IDLE) return TransitionRule::ALLOWED;
            if (to == ROS_ERROR_RECOVERING) return TransitionRule::REQUIRES_CLEANUP;
            return TransitionRule::FORBIDDEN;
            
        case ROS_PAUSED:
            if (to == ROS_STREAMING || to == ROS_IDLE) return TransitionRule::ALLOWED;
            if (to == ROS_ERROR_RECOVERING) return TransitionRule::REQUIRES_CLEANUP;
            return TransitionRule::FORBIDDEN;
            
        case ROS_ERROR_RECOVERING:
            if (to == ROS_CONNECTED || to == ROS_IDLE) return TransitionRule::CONDITIONAL;
            if (to == ROS_ERROR) return TransitionRule::ALLOWED;
            return TransitionRule::FORBIDDEN;
            
        case ROS_ERROR:
            if (to == ROS_IDLE || to == ROS_DEEP_SLEEP) return TransitionRule::ALLOWED;
            return TransitionRule::FORBIDDEN;
            
        case ROS_DEEP_SLEEP:
            if (to == ROS_IDLE) return TransitionRule::ALLOWED;
            return TransitionRule::FORBIDDEN;
            
        default:
            return TransitionRule::FORBIDDEN;
    }
}

bool ROSStateMachine::hasStateChanged() {
    if (!_initialized) return false;
    
    bool changed = _stateChangeFlag;
    _clearStateChangeFlag();
    return changed;
}

bool ROSStateMachine::isTransitionPending() {
    bool pending = _transitionPendingFlag;
    _transitionPendingFlag = false;
    return pending;
}

bool ROSStateMachine::transitionTo(ROSStateMachine::State newState, uint32_t currentTimeMs) {
    if (!_initialized) {
        return false;
    }

    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();
    
    // Check if this is a real state change
    if (_currentState == newState) {
        return true;
    }
    
    // Validate transition
    if (!isValidTransition(_currentState, newState)) {
        _transitionPendingFlag = true;
        return false;
    }
    
    // Execute pre-transition callbacks
    if (_preTransitionCallback) {
        _preTransitionCallback(_currentState, newState);
    }
    
    // Record transition
    _previousState = _currentState;
    _stateEntryTime = now;
    _stateChangeFlag = true;
    
    // Update state
    _currentState = newState;
    
    // Execute post-transition callbacks
    if (_postTransitionCallback) {
        _postTransitionCallback(_previousState, _currentState);
    }
    
    // Call main state change callback
    if (_stateChangeCallback) {
        _stateChangeCallback(_previousState, _currentState);
    }
    
    // Record in history
    _recordTransition(_previousState, _currentState, now);
    
    return true;
}

void ROSStateMachine::onButtonPress(uint32_t currentTimeMs) {
    if (!_initialized) return;
    
    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();
    
    switch (_currentState) {
        case ROS_IDLE:
            // Request connection
            transitionTo(ROS_CONNECTING, now);
            break;
            
        case ROS_CONNECTING:
            // Cancel connection attempt
            transitionTo(ROS_IDLE, now);
            break;
            
        case ROS_CONNECTED:
            // Start streaming if not already
            if (_currentState != ROS_STREAMING) {
                transitionTo(ROS_STREAMING, now);
            }
            break;
            
        case ROS_STREAMING:
            // Pause streaming
            transitionTo(ROS_PAUSED, now);
            break;
            
        case ROS_PAUSED:
            // Resume streaming
            transitionTo(ROS_STREAMING, now);
            break;
            
        case ROS_ERROR:
        case ROS_ERROR_RECOVERING:
            // Attempt recovery
            transitionTo(ROS_ERROR_RECOVERING, now);
            break;
            
        default:
            break;
    }
}

void ROSStateMachine::onBLEConnect(uint32_t currentTimeMs) {
    if (!_initialized) return;
    
    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();
    
    switch (_currentState) {
        case ROS_CONNECTING:
            _bleConnected = true;
            transitionTo(ROS_CONNECTED, now);
            break;
            
        case ROS_IDLE:
            // Auto-connect on button press
            _bleConnected = true;
            transitionTo(ROS_CONNECTING, now);
            break;
            
        default:
            break;
    }
}

void ROSStateMachine::onBLEDisconnect(uint32_t currentTimeMs) {
    if (!_initialized) return;
    
    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();
    _bleConnected = false;
    
    switch (_currentState) {
        case ROS_CONNECTED:
            transitionTo(ROS_IDLE, now);
            break;
            
        case ROS_STREAMING:
        case ROS_PAUSED:
            transitionTo(ROS_ERROR, now);
            break;
            
        default:
            break;
    }
}

void ROSStateMachine::onError(const char* errorReason, uint32_t currentTimeMs) {
    if (!_initialized) return;

    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();

    // Determine error severity based on current state
    ErrorSeverity severity = ERROR_MAJOR;
    switch (_currentState) {
        case ROS_CONNECTING:
            severity = ERROR_MINOR;
            break;
        case ROS_ERROR_RECOVERING:
            severity = ERROR_WARNING;
            break;
        case ROS_STREAMING:
        case ROS_PAUSED:
            severity = ERROR_CRITICAL;
            break;
        default:
            severity = ERROR_MAJOR;
            break;
    }

    setLastError(errorReason, severity);

    switch (_currentState) {
        case ROS_CONNECTING:
            transitionTo(ROS_ERROR_RECOVERING, now);
            break;

        case ROS_CONNECTED:
        case ROS_STREAMING:
        case ROS_PAUSED:
            transitionTo(ROS_ERROR, now);
            break;

        default:
            break;
    }
}

void ROSStateMachine::onWatchdogTimeout(uint32_t currentTimeMs) {
    if (!_initialized) return;
    
    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();
    
    // If in streaming or connected state, error recovery
    if (_currentState == ROS_STREAMING || _currentState == ROS_PAUSED) {
        transitionTo(ROS_ERROR_RECOVERING, now);
    }
}

void ROSStateMachine::onConnectionTimeout(uint32_t currentTimeMs) {
    if (!_initialized) return;
    
    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();
    
    if (_currentState == ROS_CONNECTING) {
        transitionTo(ROS_ERROR_RECOVERING, now);
    }
}

void ROSStateMachine::setConfig(const Config& config) {
    _config = config;
}

const ROSStateMachine::Config& ROSStateMachine::getConfig() const {
    return _config;
}

void ROSStateMachine::setStateChangeCallback(StateChangeCallback callback) {
    _stateChangeCallback = callback;
}

void ROSStateMachine::setTransitionValidator(TransitionValidator validator) {
    _transitionValidator = validator;
}

void ROSStateMachine::setPreTransitionCallback(PreTransitionCallback callback) {
    _preTransitionCallback = callback;
}

void ROSStateMachine::setPostTransitionCallback(PostTransitionCallback callback) {
    _postTransitionCallback = callback;
}

void ROSStateMachine::reset() {
    if (!_initialized) return;
    
    uint32_t now = millis();
    transitionTo(ROS_IDLE, now);
    _bleConnected = false;
}

void ROSStateMachine::attemptRecovery() {
    if (!_initialized) return;
    
    uint32_t now = millis();
    
    if (_currentState == ROS_ERROR) {
        transitionTo(ROS_IDLE, now);
    } else if (_currentState == ROS_ERROR_RECOVERING) {
        // Already recovering, allow retry
        _config.maxRetries--;
        if (_config.maxRetries > 0) {
            transitionTo(ROS_IDLE, now);
        } else {
            transitionTo(ROS_DEEP_SLEEP, now);
        }
    }
}

void ROSStateMachine::_clearStateChangeFlag() {
    _stateChangeFlag = false;
}

void ROSStateMachine::_recordTransition(State from, State to, uint32_t timestamp) {
    _transitionHistory[_transitionHistoryIndex].from = from;
    _transitionHistory[_transitionHistoryIndex].to = to;
    _transitionHistory[_transitionHistoryIndex].timestamp = timestamp;

    _transitionHistoryIndex = (_transitionHistoryIndex + 1) % MAX_TRANSITION_HISTORY;
}

// Design Improvements: Statistics and Analytics Implementation

uint8_t ROSStateMachine::_getTransitionStatsIndex(State from, State to) const {
    // Simple hash function to map state pairs to stats index
    uint16_t hash = ((uint16_t)from << 8) | (uint16_t)to;
    return hash % MAX_TRANSITION_STATS;
}

void ROSStateMachine::_updateStateStats(State state, uint32_t dwellTime) {
    StateStats& stats = _stateStats[state];
    stats.visitCount++;
    stats.totalDwellTimeMs += dwellTime;
    
    if (dwellTime < stats.minDwellTimeMs) {
        stats.minDwellTimeMs = dwellTime;
    }
    if (dwellTime > stats.maxDwellTimeMs) {
        stats.maxDwellTimeMs = dwellTime;
    }
}

void ROSStateMachine::_updateTransitionStats(State from, State to, bool success) {
    uint8_t index = _getTransitionStatsIndex(from, to);
    TransitionStats& stats = _transitionStats[index];
    stats.transitionCount++;
    
    if (success) {
        stats.successCount++;
    } else {
        stats.rejectedCount++;
    }
}

const ROSStateMachine::StateStats& ROSStateMachine::getStateStats(State state) const {
    return _stateStats[state];
}

const ROSStateMachine::TransitionStats& ROSStateMachine::getTransitionStats(State from, State to) const {
    uint8_t index = _getTransitionStatsIndex(from, to);
    return _transitionStats[index];
}

ROSStateMachine::StateStats ROSStateMachine::getAverageStats() const {
    StateStats avg;
    avg.visitCount = 0;
    avg.totalDwellTimeMs = 0;
    avg.minDwellTimeMs = UINT32_MAX;
    avg.maxDwellTimeMs = 0;

    uint32_t activeStates = 0;
    for (uint8_t i = 0; i < MAX_STATE_STATS; i++) {
        if (_stateStats[i].visitCount > 0) {
            avg.visitCount += _stateStats[i].visitCount;
            avg.totalDwellTimeMs += _stateStats[i].totalDwellTimeMs;
            if (_stateStats[i].minDwellTimeMs < avg.minDwellTimeMs) {
                avg.minDwellTimeMs = _stateStats[i].minDwellTimeMs;
            }
            if (_stateStats[i].maxDwellTimeMs > avg.maxDwellTimeMs) {
                avg.maxDwellTimeMs = _stateStats[i].maxDwellTimeMs;
            }
            activeStates++;
        }
    }

    if (activeStates > 0) {
        avg.minDwellTimeMs = (avg.minDwellTimeMs == UINT32_MAX) ? 0 : avg.minDwellTimeMs;
        avg.maxDwellTimeMs = (avg.maxDwellTimeMs == 0) ? 0 : avg.maxDwellTimeMs;
    }

    return avg;
}

void ROSStateMachine::resetStats() {
    // Reset state statistics
    for (uint8_t i = 0; i < MAX_STATE_STATS; i++) {
        _stateStats[i].visitCount = 0;
        _stateStats[i].totalDwellTimeMs = 0;
        _stateStats[i].minDwellTimeMs = UINT32_MAX;
        _stateStats[i].maxDwellTimeMs = 0;
    }

    // Reset transition statistics
    for (uint8_t i = 0; i < MAX_TRANSITION_STATS; i++) {
        _transitionStats[i].transitionCount = 0;
        _transitionStats[i].successCount = 0;
        _transitionStats[i].rejectedCount = 0;
    }

    _transitionStatsIndex = 0;
}

// Design Improvements: Error Handling Implementation

ROSStateMachine::ErrorSeverity ROSStateMachine::getCurrentErrorSeverity() const {
    return _lastErrorSeverity;
}

const char* ROSStateMachine::getErrorSeverityName(ErrorSeverity severity) const {
    switch (severity) {
        case ERROR_NONE: return "NONE";
        case ERROR_WARNING: return "WARNING";
        case ERROR_MINOR: return "MINOR";
        case ERROR_MAJOR: return "MAJOR";
        case ERROR_CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void ROSStateMachine::setLastError(const char* errorReason, ErrorSeverity severity) {
    _lastErrorMessage = errorReason;
    _lastErrorSeverity = severity;
    _errorCount++;
}

const char* ROSStateMachine::getLastErrorMessage() const {
    return _lastErrorMessage ? _lastErrorMessage : "No errors recorded";
}

ROSStateMachine::ErrorSeverity ROSStateMachine::getLastErrorMessageSeverity() const {
    return _lastErrorSeverity;
}

// Override transitionTo to record statistics
bool ROSStateMachine::transitionTo(ROSStateMachine::State newState, uint32_t currentTimeMs) {
    if (!_initialized) {
        return false;
    }

    uint32_t now = (currentTimeMs > 0) ? currentTimeMs : millis();

    // Check if this is a real state change
    if (_currentState == newState) {
        return true;
    }

    // Record dwell time for current state
    uint32_t dwellTime = (now >= _stateEntryTime) ? (now - _stateEntryTime) : 0;
    _updateStateStats(_currentState, dwellTime);

    // Validate transition
    if (!isValidTransition(_currentState, newState)) {
        _transitionPendingFlag = true;
        _updateTransitionStats(_currentState, newState, false);
        return false;
    }

    // Execute pre-transition callbacks
    if (_preTransitionCallback) {
        _preTransitionCallback(_currentState, newState);
    }

    // Record transition
    _previousState = _currentState;
    _stateEntryTime = now;
    _stateChangeFlag = true;

    // Update state
    _currentState = newState;

    // Record successful transition statistics
    _updateTransitionStats(_previousState, _currentState, true);

    // Execute post-transition callbacks
    if (_postTransitionCallback) {
        _postTransitionCallback(_previousState, _currentState);
    }

    // Call main state change callback
    if (_stateChangeCallback) {
        _stateChangeCallback(_previousState, _currentState);
    }

    // Record in history
    _recordTransition(_previousState, _currentState, now);

    return true;
}
