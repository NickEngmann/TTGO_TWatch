#include <unity.h>
#include <cstdint>
#include "../../src/ROSStateMachine.cpp"

// Test callbacks for tracking state changes
static bool stateChangeCallbackCalled = false;
static ROSStateMachine::State lastOldState = ROSStateMachine::ROS_IDLE;
static ROSStateMachine::State lastNewState = ROSStateMachine::ROS_IDLE;

static bool preTransitionCallbackCalled = false;
static bool postTransitionCallbackCalled = false;

static void testStateChangeCallback(ROSStateMachine::State oldState, ROSStateMachine::State newState) {
    stateChangeCallbackCalled = true;
    lastOldState = oldState;
    lastNewState = newState;
}

static void testPreTransitionCallback(ROSStateMachine::State from, ROSStateMachine::State to) {
    preTransitionCallbackCalled = true;
}

static void testPostTransitionCallback(ROSStateMachine::State from, ROSStateMachine::State to) {
    postTransitionCallbackCalled = true;
}

void setUp(void) {
    stateChangeCallbackCalled = false;
    lastOldState = ROSStateMachine::ROS_IDLE;
    lastNewState = ROSStateMachine::ROS_IDLE;
    preTransitionCallbackCalled = false;
    postTransitionCallbackCalled = false;
}

void tearDown(void) {}

// Test: Initialization
void test_init_success(void) {
    ROSStateMachine sm;
    bool result = sm.init();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, sm.getCurrentState());
}

// Test: State machine starts in IDLE state
void test_initial_state_is_idle(void) {
    ROSStateMachine sm;
    sm.init();
    TEST_ASSERT_EQUAL_STRING("IDLE", sm.getStateName());
}

// Test: State transition from IDLE to CONNECTING
void test_transition_idle_to_connecting(void) {
    ROSStateMachine sm;
    sm.init();
    
    bool result = sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_CONNECTING, sm.getCurrentState());
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, sm.getPreviousState());
}

// Test: State transition from CONNECTING to CONNECTED
void test_transition_connecting_to_connected(void) {
    ROSStateMachine sm;
    sm.init();
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    
    bool result = sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_CONNECTED, sm.getCurrentState());
}

// Test: State transition from CONNECTED to STREAMING
void test_transition_connected_to_streaming(void) {
    ROSStateMachine sm;
    sm.init();
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 1500);
    
    bool result = sm.transitionTo(ROSStateMachine::ROS_STREAMING, 2000);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_STREAMING, sm.getCurrentState());
}

// Test: Invalid transition should fail
void test_invalid_transition_fails(void) {
    ROSStateMachine sm;
    sm.init();
    
    // Cannot transition directly from IDLE to STREAMING
    bool result = sm.transitionTo(ROSStateMachine::ROS_STREAMING, 1000);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, sm.getCurrentState());
}

// Test: State transition with callbacks
void test_state_transition_with_callbacks(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.setStateChangeCallback(testStateChangeCallback);
    sm.setPreTransitionCallback(testPreTransitionCallback);
    sm.setPostTransitionCallback(testPostTransitionCallback);
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    
    TEST_ASSERT_TRUE(stateChangeCallbackCalled);
    TEST_ASSERT_TRUE(preTransitionCallbackCalled);
    TEST_ASSERT_TRUE(postTransitionCallbackCalled);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, lastOldState);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_CONNECTING, lastNewState);
}

// Test: Edge-triggered state change flag
void test_edge_triggered_state_change(void) {
    ROSStateMachine sm;
    sm.init();
    
    // Initially should be false
    TEST_ASSERT_FALSE(sm.hasStateChanged());
    
    // After state change, should be true
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    TEST_ASSERT_TRUE(sm.hasStateChanged());
    
    // Should be cleared after reading
    TEST_ASSERT_FALSE(sm.hasStateChanged());
}

// Test: Button press transitions
void test_button_press_idle_to_connecting(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.onButtonPress(1000);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_CONNECTING, sm.getCurrentState());
}

// Test: Button press during streaming toggles pause
void test_button_press_streaming_to_paused(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 500);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 1000);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 1500);
    
    sm.onButtonPress(2000);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_PAUSED, sm.getCurrentState());
}

// Test: BLE connect transition
void test_ble_connect_idling(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.onBLEConnect(1000);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_CONNECTING, sm.getCurrentState());
}

// Test: BLE disconnect transitions
void test_ble_disconnect_connected_to_idle(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 500);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 1000);
    
    sm.onBLEDisconnect(1500);
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, sm.getCurrentState());
}

// Test: Error transition
void test_error_transition(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 500);
    sm.onError("Connection failed", 1000);
    
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_ERROR, sm.getCurrentState());
}

// Test: Error recovery
void test_error_recovery(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 500);
    sm.onError("Connection failed", 1000);
    sm.attemptRecovery();
    
    // Should transition back to IDLE
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, sm.getCurrentState());
}

// Test: Transition validity checking
void test_transition_validity(void) {
    ROSStateMachine sm;
    sm.init();
    
    TEST_ASSERT_TRUE(sm.isValidTransition(ROSStateMachine::ROS_IDLE, 
                                          ROSStateMachine::ROS_CONNECTING));
    TEST_ASSERT_TRUE(sm.isValidTransition(ROSStateMachine::ROS_IDLE, 
                                          ROSStateMachine::ROS_IDLE));
    TEST_ASSERT_FALSE(sm.isValidTransition(ROSStateMachine::ROS_IDLE, 
                                           ROSStateMachine::ROS_STREAMING));
    TEST_ASSERT_FALSE(sm.isValidTransition(ROSStateMachine::ROS_CONNECTED, 
                                           ROSStateMachine::ROS_IDLE));
}

// Test: Configuration settings
void test_configuration(void) {
    ROSStateMachine sm;
    sm.init();
    
    ROSStateMachine::Config customConfig;
    customConfig.idleTimeoutMs = 60000;
    customConfig.errorRecoveryMs = 10000;
    customConfig.watchdogTimeoutMs = 120000;
    
    sm.setConfig(customConfig);
    const ROSStateMachine::Config& retrieved = sm.getConfig();
    
    TEST_ASSERT_EQUAL_UINT32(60000, retrieved.idleTimeoutMs);
    TEST_ASSERT_EQUAL_UINT32(10000, retrieved.errorRecoveryMs);
    TEST_ASSERT_EQUAL_UINT32(120000, retrieved.watchdogTimeoutMs);
}

// Test: State machine reset
void test_reset(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    sm.reset();
    
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_IDLE, sm.getCurrentState());
}

// Test: Connection timeout handling
void test_connection_timeout(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.onConnectionTimeout(12000);  // After 10s timeout
    
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_ERROR_RECOVERING, 
                      sm.getCurrentState());
}

// Test: Watchdog timeout handling
void test_watchdog_timeout(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 3000);
    
    sm.onWatchdogTimeout(65000);  // After 60s watchdog
    
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_ERROR_RECOVERING, 
                      sm.getCurrentState());
}

// Test: Transition history recording
void test_transition_history(void) {
    ROSStateMachine sm;
    sm.init();
    
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 3000);
    
    // State should be correct
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_STREAMING, sm.getCurrentState());
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_CONNECTED, sm.getPreviousState());
}

// Test: Deep sleep state
void test_deep_sleep_state(void) {
    ROSStateMachine sm;
    sm.init();
    
    // Error state can transition to deep sleep
    sm.transitionTo(ROSStateMachine::ROS_ERROR, 1000);
    sm.transitionTo(ROSStateMachine::ROS_DEEP_SLEEP, 2000);
    
    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_DEEP_SLEEP, sm.getCurrentState());
    TEST_ASSERT_EQUAL_STRING("DEEP_SLEEP", sm.getStateName());
}

// Test: Paused to streaming transition
void test_paused_to_streaming(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 1500);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 2000);
    sm.transitionTo(ROSStateMachine::ROS_PAUSED, 2500);

    // Button press resumes streaming
    sm.onButtonPress(3000);

    TEST_ASSERT_EQUAL(ROSStateMachine::ROS_STREAMING, sm.getCurrentState());
}

// Test: Statistics - state visit tracking
void test_state_stats_visit_count(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 3000);
    sm.transitionTo(ROSStateMachine::ROS_PAUSED, 4000);

    const auto& stats = sm.getStateStats(ROSStateMachine::ROS_STREAMING);
    TEST_ASSERT_GREATER_THAN(0, stats.visitCount);
}

// Test: Statistics - transition success tracking
void test_transition_stats_success(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);

    const auto& transStats = sm.getTransitionStats(ROS_IDLE, ROS_CONNECTING);
    TEST_ASSERT_GREATER_THAN(0, transStats.transitionCount);
    TEST_ASSERT_EQUAL(transStats.transitionCount, transStats.successCount);
}

// Test: Statistics - invalid transition tracking
void test_transition_stats_rejected(void) {
    ROSStateMachine sm;
    sm.init();

    // Try invalid transition
    bool result = sm.transitionTo(ROSStateMachine::ROS_STREAMING, 1000);
    TEST_ASSERT_FALSE(result);

    const auto& transStats = sm.getTransitionStats(ROS_IDLE, ROS_STREAMING);
    TEST_ASSERT_EQUAL(1, transStats.transitionCount);
    TEST_ASSERT_EQUAL(1, transStats.rejectedCount);
}

// Test: Statistics - reset stats functionality
void test_reset_stats(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);

    sm.resetStats();

    const auto& stats = sm.getStateStats(ROSStateMachine::ROS_IDLE);
    TEST_ASSERT_EQUAL(0, stats.visitCount);
}

// Test: Error handling - severity tracking
void test_error_severity_streaming_critical(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 3000);

    sm.onError("Audio stream interrupted", 4000);

    TEST_ASSERT_EQUAL(ROSStateMachine::ErrorSeverity::ERROR_CRITICAL,
                      sm.getCurrentErrorSeverity());
}

// Test: Error handling - severity tracking during connecting
void test_error_severity_connecting_minor(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);

    sm.onError("BLE handshake timeout", 2000);

    TEST_ASSERT_EQUAL(ROSStateMachine::ErrorSeverity::ERROR_MINOR,
                      sm.getCurrentErrorSeverity());
}

// Test: Error handling - error message retrieval
void test_error_message_retrieval(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.onError("Test error message", 2000);

    TEST_ASSERT_EQUAL_STRING("Test error message", sm.getLastErrorMessage());
}

// Test: Error handling - severity name conversion
void test_error_severity_name(void) {
    ROSStateMachine sm;
    
    TEST_ASSERT_EQUAL_STRING("NONE", sm.getErrorSeverityName(ROSStateMachine::ErrorSeverity::ERROR_NONE));
    TEST_ASSERT_EQUAL_STRING("WARNING", sm.getErrorSeverityName(ROSStateMachine::ErrorSeverity::ERROR_WARNING));
    TEST_ASSERT_EQUAL_STRING("MINOR", sm.getErrorSeverityName(ROSStateMachine::ErrorSeverity::ERROR_MINOR));
    TEST_ASSERT_EQUAL_STRING("MAJOR", sm.getErrorSeverityName(ROSStateMachine::ErrorSeverity::ERROR_MAJOR));
    TEST_ASSERT_EQUAL_STRING("CRITICAL", sm.getErrorSeverityName(ROSStateMachine::ErrorSeverity::ERROR_CRITICAL));
}

// Test: Error handling - error count tracking
void test_error_count_tracking(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.onError("Error 1", 2000);
    sm.onError("Error 2", 3000);

    // Verify error severity changed appropriately
    TEST_ASSERT_EQUAL(ROSStateMachine::ErrorSeverity::ERROR_MAJOR,
                      sm.getLastErrorMessageSeverity());
}

// Test: Average statistics calculation
void test_average_statistics(void) {
    ROSStateMachine sm;
    sm.init();

    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 2000);
    sm.transitionTo(ROSStateMachine::ROS_STREAMING, 3000);

    auto avgStats = sm.getAverageStats();
    TEST_ASSERT_GREATER_THAN(0, avgStats.visitCount);
    TEST_ASSERT_EQUAL(avgStats.visitCount, 3); // 3 state visits
}

// Test: State dwell time tracking
void test_dwell_time_tracking(void) {
    ROSStateMachine sm;
    sm.init();

    // Stay in CONNECTING for 100ms
    sm.transitionTo(ROSStateMachine::ROS_CONNECTING, 1000);
    sm.transitionTo(ROSStateMachine::ROS_CONNECTED, 1100);

    const auto& stats = sm.getStateStats(ROSStateMachine::ROS_CONNECTING);
    TEST_ASSERT_EQUAL_UINT32(100, stats.minDwellTimeMs);
    TEST_ASSERT_EQUAL_UINT32(100, stats.maxDwellTimeMs);
    TEST_ASSERT_EQUAL_UINT32(100, stats.totalDwellTimeMs);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_init_success);
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_transition_idle_to_connecting);
    RUN_TEST(test_transition_connecting_to_connected);
    RUN_TEST(test_transition_connected_to_streaming);
    RUN_TEST(test_invalid_transition_fails);
    RUN_TEST(test_state_transition_with_callbacks);
    RUN_TEST(test_edge_triggered_state_change);
    RUN_TEST(test_button_press_idle_to_connecting);
    RUN_TEST(test_button_press_streaming_to_paused);
    RUN_TEST(test_ble_connect_idling);
    RUN_TEST(test_ble_disconnect_connected_to_idle);
    RUN_TEST(test_error_transition);
    RUN_TEST(test_error_recovery);
    RUN_TEST(test_transition_validity);
    RUN_TEST(test_configuration);
    RUN_TEST(test_reset);
    RUN_TEST(test_connection_timeout);
    RUN_TEST(test_watchdog_timeout);
    RUN_TEST(test_transition_history);
    RUN_TEST(test_deep_sleep_state);
    RUN_TEST(test_paused_to_streaming);

    // New tests for design improvements
    RUN_TEST(test_state_stats_visit_count);
    RUN_TEST(test_transition_stats_success);
    RUN_TEST(test_transition_stats_rejected);
    RUN_TEST(test_reset_stats);
    RUN_TEST(test_error_severity_streaming_critical);
    RUN_TEST(test_error_severity_connecting_minor);
    RUN_TEST(test_error_message_retrieval);
    RUN_TEST(test_error_severity_name);
    RUN_TEST(test_error_count_tracking);
    RUN_TEST(test_average_statistics);
    RUN_TEST(test_dwell_time_tracking);

    return UNITY_END();
}
