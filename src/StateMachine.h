#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>
#else
#include <Arduino.h>
#endif

class StateMachine {
public:
    enum State {
        IDLE,
        RECORDING,
        STREAMING,
        ERROR,
        DEEP_SLEEP
    };

    struct Config {
        uint32_t idleTimeoutMs = 30000;
        uint32_t errorRecoveryMs = 5000;
        uint32_t watchdogTimeoutMs = 60000;
    };

    using StateChangeCallback = std::function<void(State oldState, State newState)>;

    StateMachine();
    ~StateMachine();

    bool init();
    void update(uint32_t currentTimeMs);

    // State queries
    State getCurrentState() const;
    const char* getStateName() const;
    uint32_t getTimeInState(uint32_t currentTimeMs) const;

    // Events
    void onButtonPress();
    void onBLEConnect();
    void onBLEDisconnect();
    void onError();

    // Config
    void setConfig(const Config& config);
    const Config& getConfig() const;
    void setStateChangeCallback(StateChangeCallback callback);

    // Requests
    void requestRecording();
    void requestStreaming();
    void requestIdle();

private:
    State _state;
    State _previousState;
    Config _config;
    uint32_t _stateEntryTime;
    uint32_t _lastActivityTime;
    bool _initialized;
    bool _bleConnected;
    StateChangeCallback _callback;

    void _transitionTo(State newState, uint32_t currentTimeMs = 0);
};

#endif // STATE_MACHINE_H
