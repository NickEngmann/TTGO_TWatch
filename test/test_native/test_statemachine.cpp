/*
 * test_statemachine.cpp - Unity tests for StateMachine
 * Tests state machine logic without hardware dependencies
 */

#include <unity.h>
#include <cstdlib>
#include <ctime>
#include "../src/StateMachine.cpp"

// Mock millis() for native builds
static uint32_t mockMillis = 0;

uint32_t millis() {
    return mockMillis;
}

// Test fixture setup
class StateMachineTest {
public:
    StateMachine stateMachine;
    StateMachineConfig defaultConfig;
    
    void setUp() {
        defaultConfig.idleTimeoutMs = 30000;
        defaultConfig.errorRecoveryMs = 5000;
        defaultConfig.watchdogTimeoutMs = 10000;
        mockMillis = 0;
    }
    
    void tearDown() {
        // Cleanup if needed
    }
};

StateMachineTest testFixture;

void test_initial_state_is_idle() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    TEST_ASSERT_FALSE(testFixture.stateMachine.isRecording());
    TEST_ASSERT_FALSE(testFixture.stateMachine.isStreaming());
    TEST_ASSERT_FALSE(testFixture.stateMachine.isError());
    testFixture.tearDown();
}

void test_button_press_transitions_idle_to_recording() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    TEST_ASSERT_TRUE(testFixture.stateMachine.isRecording());
    testFixture.tearDown();
}

void test_button_press_transitions_recording_to_idle() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    testFixture.stateMachine.handleButtonPress();
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    TEST_ASSERT_FALSE(testFixture.stateMachine.isRecording());
    testFixture.tearDown();
}

void test_ble_connect_transitions_recording_to_streaming() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::STREAMING, testFixture.stateMachine.getCurrentState());
    TEST_ASSERT_TRUE(testFixture.stateMachine.isStreaming());
    testFixture.tearDown();
}

void test_ble_disconnect_transitions_streaming_to_recording() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::STREAMING, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(false);
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_idle_timeout_transitions_to_deep_sleep() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    // Advance time past idle timeout
    mockMillis = testFixture.defaultConfig.idleTimeoutMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::DEEP_SLEEP, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_error_state_auto_recovery() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.setError("Test error");
    
    TEST_ASSERT_EQUAL(SystemState::ERROR, testFixture.stateMachine.getCurrentState());
    
    // Advance time past error recovery timeout
    mockMillis = testFixture.defaultConfig.errorRecoveryMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_button_press_clears_error_state() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.setError("Test error");
    
    TEST_ASSERT_EQUAL(SystemState::ERROR, testFixture.stateMachine.getCurrentState());
    
    // Button press should clear error
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_watchdog_expiration_detection() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    // Advance time past watchdog timeout
    mockMillis = testFixture.defaultConfig.watchdogTimeoutMs + 1000;
    
    // Feed watchdog at start
    testFixture.stateMachine.feedWatchdog();
    
    // Should be expired now
    TEST_ASSERT_TRUE(testFixture.stateMachine.isWatchdogExpired());
    testFixture.tearDown();
}

void test_watchdog_feed_resets_timer() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    // Feed watchdog
    testFixture.stateMachine.feedWatchdog();
    
    // Advance time but not past timeout
    mockMillis = testFixture.defaultConfig.watchdogTimeoutMs / 2;
    
    // Should not be expired
    TEST_ASSERT_FALSE(testFixture.stateMachine.isWatchdogExpired());
    testFixture.tearDown();
}

void test_state_change_callback_invoked() {
    testFixture.setUp();
    bool callbackInvoked = false;
    SystemState lastState = SystemState::IDLE;
    
    StateMachineConfig config = testFixture.defaultConfig;
    // Note: stateChangeCallback is not in default config, testing basic functionality
        callbackInvoked = true;
        lastState = newState;
    };
    
    testFixture.stateMachine.init(config);
    testFixture.stateMachine.handleButtonPress();
    
    TEST_ASSERT_TRUE(callbackInvoked);
    TEST_ASSERT_EQUAL(SystemState::RECORDING, lastState);
    testFixture.tearDown();
}

void test_deep_sleep_wake_on_button_press() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    // Simulate deep sleep timeout
    mockMillis = testFixture.defaultConfig.idleTimeoutMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::DEEP_SLEEP, testFixture.stateMachine.getCurrentState());
    
    // Button press should wake from deep sleep
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_streaming_to_recording_on_ble_disconnect() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::STREAMING, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(false);
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_clear_error_resets_state() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.setError("Test error");
    
    TEST_ASSERT_EQUAL(SystemState::ERROR, testFixture.stateMachine.getCurrentState());
    
    // Clear error
    testFixture.stateMachine.clearError();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_update_does_not_crash_with_no_init() {
    testFixture.setUp();
    
    // Don't call init, just call update
    testFixture.stateMachine.update(1000);
    
    // Should not crash or have invalid state
    TEST_ASSERT_TRUE(true);
    testFixture.tearDown();
}

void test_multiple_button_presses_toggle_recording() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    // Press button to start recording
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    
    // Press button to stop recording
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    
    // Press button to start recording again
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_ble_connect_while_idle_does_not_change_state() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    
    // Connect BLE while idle
    testFixture.stateMachine.setBLEConnected(true);
    
    // Should still be idle
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}



void test_idle_timeout_transitions_to_deep_sleep() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    // Advance time past idle timeout
    mockMillis = testFixture.defaultConfig.idleTimeoutMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::DEEP_SLEEP, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_error_state_auto_recovery() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.setError("Test error");
    TEST_ASSERT_EQUAL(SystemState::ERROR, testFixture.stateMachine.getCurrentState());
    
    // Advance time past error recovery timeout
    mockMillis = testFixture.defaultConfig.errorRecoveryMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    TEST_ASSERT_FALSE(testFixture.stateMachine.isError());
    testFixture.tearDown();
}

void test_button_press_clears_error_state() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.setError("Test error");
    TEST_ASSERT_EQUAL(SystemState::ERROR, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_watchdog_expiration_detection() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.feedWatchdog();
    
    // Advance time past watchdog timeout
    mockMillis = testFixture.defaultConfig.watchdogTimeoutMs + 1000;
    
    TEST_ASSERT_TRUE(testFixture.stateMachine.isWatchdogExpired());
    testFixture.tearDown();
}

void test_watchdog_feed_resets_timer() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.feedWatchdog();
    
    // Advance time past watchdog timeout
    mockMillis = testFixture.defaultConfig.watchdogTimeoutMs + 1000;
    testFixture.stateMachine.feedWatchdog();
    
    TEST_ASSERT_FALSE(testFixture.stateMachine.isWatchdogExpired());
    testFixture.tearDown();
}

void test_state_change_callback_invoked() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    SystemState lastOldState = SystemState::IDLE;
    SystemState lastNewState = SystemState::IDLE;
    
    testFixture.stateMachine.onStateChange([&lastOldState, &lastNewState](SystemState oldState, SystemState newState) {
        lastOldState = oldState;
        lastNewState = newState;
    });
    
    testFixture.stateMachine.handleButtonPress();
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, lastOldState);
    TEST_ASSERT_EQUAL(SystemState::RECORDING, lastNewState);
    testFixture.tearDown();
}

void test_deep_sleep_wake_on_button_press() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.requestDeepSleep();
    TEST_ASSERT_EQUAL(SystemState::DEEP_SLEEP, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_streaming_to_recording_on_ble_disconnect() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::STREAMING, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(false);
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_clear_error_resets_state() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.setError("Test error");
    TEST_ASSERT_EQUAL(SystemState::ERROR, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.clearError();
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    TEST_ASSERT_FALSE(testFixture.stateMachine.isError());
    testFixture.tearDown();
}

void test_update_does_not_crash_with_no_init() {
    testFixture.setUp();
    // Don't call init, just test update
    mockMillis = 1000;
    testFixture.stateMachine.update(mockMillis);
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_multiple_button_presses_toggle_recording() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_TRUE(testFixture.stateMachine.isRecording());
    
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_FALSE(testFixture.stateMachine.isRecording());
    
    testFixture.stateMachine.handleButtonPress();
    TEST_ASSERT_TRUE(testFixture.stateMachine.isRecording());
    testFixture.tearDown();
}

void test_ble_connect_while_idle_does_not_change_state() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_streaming_to_recording_on_ble_disconnect_then_back_to_streaming() {
    testFixture.setUp();
    testFixture.stateMachine.init(testFixture.defaultConfig);
    testFixture.stateMachine.handleButtonPress();
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::STREAMING, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(false);
    TEST_ASSERT_EQUAL(SystemState::RECORDING, testFixture.stateMachine.getCurrentState());
    
    testFixture.stateMachine.setBLEConnected(true);
    TEST_ASSERT_EQUAL(SystemState::STREAMING, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_error_recovery_timeout_configurable() {
    testFixture.setUp();
    StateMachineConfig customConfig;
    customConfig.idleTimeoutMs = 30000;
    customConfig.errorRecoveryMs = 2000; // Shorter timeout for testing
    customConfig.watchdogTimeoutMs = 10000;
    
    testFixture.stateMachine.init(customConfig);
    testFixture.stateMachine.setError("Test error");
    
    // Advance time past custom error recovery timeout
    mockMillis = customConfig.errorRecoveryMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::IDLE, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void test_idle_timeout_configurable() {
    testFixture.setUp();
    StateMachineConfig customConfig;
    customConfig.idleTimeoutMs = 5000; // Short timeout for testing
    customConfig.errorRecoveryMs = 5000;
    customConfig.watchdogTimeoutMs = 10000;
    
    testFixture.stateMachine.init(customConfig);
    
    // Advance time past custom idle timeout
    mockMillis = customConfig.idleTimeoutMs + 1000;
    testFixture.stateMachine.update(mockMillis);
    
    TEST_ASSERT_EQUAL(SystemState::DEEP_SLEEP, testFixture.stateMachine.getCurrentState());
    testFixture.tearDown();
}

void run_all_tests() {
    UNITY_BEGIN();
    
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_button_press_transitions_idle_to_recording);
    RUN_TEST(test_button_press_transitions_recording_to_idle);
    RUN_TEST(test_ble_connect_transitions_recording_to_streaming);
    RUN_TEST(test_ble_disconnect_transitions_streaming_to_recording);
    RUN_TEST(test_idle_timeout_transitions_to_deep_sleep);
    RUN_TEST(test_error_state_auto_recovery);
    RUN_TEST(test_button_press_clears_error_state);
    RUN_TEST(test_watchdog_expiration_detection);
    RUN_TEST(test_watchdog_feed_resets_timer);
    RUN_TEST(test_state_change_callback_invoked);
    RUN_TEST(test_deep_sleep_wake_on_button_press);
    RUN_TEST(test_streaming_to_recording_on_ble_disconnect);
    RUN_TEST(test_clear_error_resets_state);
    RUN_TEST(test_update_does_not_crash_with_no_init);
    RUN_TEST(test_multiple_button_presses_toggle_recording);
    RUN_TEST(test_ble_connect_while_idle_does_not_change_state);
    RUN_TEST(test_streaming_to_recording_on_ble_disconnect_then_back_to_streaming);
    RUN_TEST(test_error_recovery_timeout_configurable);
    RUN_TEST(test_idle_timeout_configurable);
    
    UNITY_END();
}

int main() {
    run_all_tests();
    return 0;
}
