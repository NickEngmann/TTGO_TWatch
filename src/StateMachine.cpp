#include "StateMachine.h"

StateMachine::StateMachine()
    : _state(IDLE), _previousState(IDLE), _stateEntryTime(0),
      _lastActivityTime(0), _initialized(false), _bleConnected(false),
      _callback(nullptr) {}

StateMachine::~StateMachine() {}

bool StateMachine::init() {
    _state = IDLE;
    _previousState = IDLE;
    _stateEntryTime = 0;
    _lastActivityTime = 0;
    _bleConnected = false;
    _initialized = true;
    return true;
}

void StateMachine::update(uint32_t currentTimeMs) {
    if (!_initialized) return;

    uint32_t elapsed = currentTimeMs - _stateEntryTime;

    switch (_state) {
        case IDLE:
            if (elapsed >= _config.idleTimeoutMs) {
                _transitionTo(DEEP_SLEEP, currentTimeMs);
            }
            break;
        case ERROR:
            if (elapsed >= _config.errorRecoveryMs) {
                _transitionTo(IDLE, currentTimeMs);
            }
            break;
        case RECORDING:
        case STREAMING:
            if (elapsed >= _config.watchdogTimeoutMs) {
                _transitionTo(ERROR, currentTimeMs);
            }
            break;
        default:
            break;
    }
}

StateMachine::State StateMachine::getCurrentState() const { return _state; }

const char* StateMachine::getStateName() const {
    switch (_state) {
        case IDLE: return "IDLE";
        case RECORDING: return "RECORDING";
        case STREAMING: return "STREAMING";
        case ERROR: return "ERROR";
        case DEEP_SLEEP: return "DEEP_SLEEP";
        default: return "UNKNOWN";
    }
}

uint32_t StateMachine::getTimeInState(uint32_t currentTimeMs) const {
    return currentTimeMs - _stateEntryTime;
}

void StateMachine::onButtonPress() {
    if (!_initialized) return;
    switch (_state) {
        case IDLE:
            _transitionTo(RECORDING);
            break;
        case RECORDING:
            _transitionTo(IDLE);
            break;
        case DEEP_SLEEP:
            _transitionTo(IDLE);
            break;
        case ERROR:
            _transitionTo(IDLE);
            break;
        default:
            break;
    }
}

void StateMachine::onBLEConnect() {
    if (!_initialized) return;
    _bleConnected = true;
    if (_state == RECORDING) {
        _transitionTo(STREAMING);
    }
}

void StateMachine::onBLEDisconnect() {
    if (!_initialized) return;
    _bleConnected = false;
    if (_state == STREAMING) {
        _transitionTo(RECORDING);
    }
}

void StateMachine::onError() {
    if (!_initialized) return;
    _transitionTo(ERROR);
}

void StateMachine::setConfig(const Config& config) { _config = config; }
const StateMachine::Config& StateMachine::getConfig() const { return _config; }

void StateMachine::setStateChangeCallback(StateChangeCallback callback) {
    _callback = callback;
}

void StateMachine::requestRecording() {
    if (_initialized && _state == IDLE) _transitionTo(RECORDING);
}

void StateMachine::requestStreaming() {
    if (_initialized && _state == RECORDING && _bleConnected) _transitionTo(STREAMING);
}

void StateMachine::requestIdle() {
    if (_initialized) _transitionTo(IDLE);
}

void StateMachine::_transitionTo(State newState, uint32_t currentTimeMs) {
    if (newState == _state) return;
    State old = _state;
    _previousState = _state;
    _state = newState;
    _stateEntryTime = currentTimeMs;
    if (_callback) _callback(old, newState);
}
