#ifndef STATE_MACHINE_ROS_H
#define STATE_MACHINE_ROS_H

#ifdef NATIVE_BUILD
#include <cstdint>
#include <functional>

// Mock millis() for native testing
inline uint32_t millis() {
    return 0;
}
#else
#include <Arduino.h>
#endif

// ROS-enhanced state machine with improved transition guards and callbacks
class ROSStateMachine {
public:
    // Enhanced state enum with additional states
    enum State {
        ROS_IDLE = 0,
        ROS_CONNECTING,
        ROS_CONNECTED,
        ROS_STREAMING,
        ROS_PAUSED,
        ROS_ERROR_RECOVERING,
        ROS_ERROR,
        ROS_DEEP_SLEEP,
        ROS_COUNT  // Must be last
    };

    // Transition rules - defines valid state transitions
    enum class TransitionRule {
        ALLOWED,
        FORBIDDEN,
        CONDITIONAL,
        REQUIRES_CLEANUP
    };

    struct Config {
        uint32_t idleTimeoutMs = 30000;
        uint32_t errorRecoveryMs = 5000;
        uint32_t watchdogTimeoutMs = 60000;
        uint32_t connectingTimeoutMs = 10000;
        bool enableEdgeTriggeredCallbacks = true;
        uint8_t maxRetries = 3;
    };

    using StateChangeCallback = std::function<void(State oldState, State newState)>;
    using TransitionValidator = std::function<bool(State from, State to)>;
    using PreTransitionCallback = std::function<void(State from, State to)>;
    using PostTransitionCallback = std::function<void(State from, State to)>;

    ROSStateMachine();
    ~ROSStateMachine();

    // Initialization
    bool init();
    void deinit();
    
    // State queries
    State getCurrentState() const;
    State getPreviousState() const;
    const char* getStateName() const;
    uint32_t getTimeInState(uint32_t currentTimeMs) const;
    
    // Transition validation
    bool isValidTransition(State from, State to) const;
    TransitionRule getTransitionRule(State from, State to) const;
    
    // Events
    void onButtonPress(uint32_t currentTimeMs);
    void onBLEConnect(uint32_t currentTimeMs);
    void onBLEDisconnect(uint32_t currentTimeMs);
    void onError(const char* errorReason, uint32_t currentTimeMs);
    void onWatchdogTimeout(uint32_t currentTimeMs);
    void onConnectionTimeout(uint32_t currentTimeMs);
    
    // State transitions (public API for manual transitions)
    bool transitionTo(State newState, uint32_t currentTimeMs = 0);
    
    // Configuration
    void setConfig(const Config& config);
    const Config& getConfig() const;
    
    // Callbacks
    void setStateChangeCallback(StateChangeCallback callback);
    void setTransitionValidator(TransitionValidator validator);
    void setPreTransitionCallback(PreTransitionCallback callback);
    void setPostTransitionCallback(PostTransitionCallback callback);
    
    // Edge-triggered events (fire once per state change)
    bool hasStateChanged();
    bool isTransitionPending();

    // Recovery
    void reset();
    void attemptRecovery();

    // Design Improvements: Statistics and Analytics
    struct StateStats {
        uint32_t visitCount = 0;
        uint32_t totalDwellTimeMs = 0;
        uint32_t minDwellTimeMs = UINT32_MAX;
        uint32_t maxDwellTimeMs = 0;
    };

    struct TransitionStats {
        uint32_t transitionCount = 0;
        uint32_t successCount = 0;
        uint32_t rejectedCount = 0;
    };

    // Retrieve statistics
    const StateStats& getStateStats(State state) const;
    const TransitionStats& getTransitionStats(State from, State to) const;
    StateStats getAverageStats() const;

    // Reset statistics counters
    void resetStats();

    // Error categorization
    enum class ErrorSeverity {
        ERROR_NONE,
        ERROR_WARNING,
        ERROR_MINOR,
        ERROR_MAJOR,
        ERROR_CRITICAL
    };

    ErrorSeverity getCurrentErrorSeverity() const;
    const char* getErrorSeverityName(ErrorSeverity severity) const;
    void setLastError(const char* errorReason, ErrorSeverity severity);
    const char* getLastErrorMessage() const;
    ErrorSeverity getLastErrorMessageSeverity() const;

private:
    State _currentState;
    State _previousState;
    Config _config;
    uint32_t _stateEntryTime;
    uint32_t _lastActivityTime;
    bool _initialized;
    bool _bleConnected;

    // Callbacks
    StateChangeCallback _stateChangeCallback;
    TransitionValidator _transitionValidator;
    PreTransitionCallback _preTransitionCallback;
    PostTransitionCallback _postTransitionCallback;

    // Edge-triggered flags
    bool _stateChangeFlag;
    bool _transitionPendingFlag;

    // Transition history for debugging
    static const uint8_t MAX_TRANSITION_HISTORY = 10;
    struct TransitionEntry {
        State from;
        State to;
        uint32_t timestamp;
    };
    TransitionEntry _transitionHistory[MAX_TRANSITION_HISTORY];
    uint8_t _transitionHistoryIndex;

    // Statistics and analytics (Design Improvements)
    static const uint8_t MAX_STATE_STATS = ROS_COUNT;
    StateStats _stateStats[MAX_STATE_STATS];
    static const uint8_t MAX_TRANSITION_STATS = 64; // Maximum possible transitions
    TransitionStats _transitionStats[MAX_TRANSITION_STATS];
    uint8_t _transitionStatsIndex;

    // Error tracking
    const char* _lastErrorMessage;
    ErrorSeverity _lastErrorSeverity;
    uint32_t _errorCount;

    // Internal methods
    void _recordTransition(State from, State to, uint32_t timestamp);
    void _clearStateChangeFlag();
    TransitionRule _determineTransitionRule(State from, State to);
    uint8_t _getTransitionStatsIndex(State from, State to) const;
};

#endif // STATE_MACHINE_ROS_H
