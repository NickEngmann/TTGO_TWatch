/*
 * test_statemachine.cpp - Unit tests for WatchStateMachine
 * 
 * Tests the state machine logic for T-Watch firmware:
 * - State transitions (IDLE -> RECORDING -> STREAMING -> ERROR)
 * - Button press handling
 * - BLE connection detection
 * - Timeout behavior
 * - Error recovery
 */

#include <unity.h>
#include <stdint.h>
#include <stdbool.h>
#include "../src/StateMachine.cpp"

// Mock millis() for native builds
uint32_t mockMillis = 0;
uint32_t millis() {
    return mockMillis;
}

// Global test state machine instance
StateMachine* testStateMachine = nullptr;

void setUp(void) {
    // Create a new state machine instance for each test
    testStateMachine = new StateMachine();
    testStateMachine->begin();
}

void tearDown(void) {
    delete testStateMachine;
    testStateMachine = nullptr;
}

// Test 1: Initial state should be IDLE
void test_initial_state_is_idle(void) {
    TEST_ASSERT_EQUAL(StateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 2: Button press transitions IDLE -> RECORDING
void test_idle_to_recording_on_button_press(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS);
    TEST_ASSERT_EQUAL(StateMachine::STATE_RECORDING, testStateMachine->getState());
}

// Test 3: Button press transitions RECORDING -> IDLE
void test_recording_to_idle_on_button_press(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS);
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS);
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 4: BLE connection transitions RECORDING -> STREAMING
void test_recording_to_streaming_on_ble_connection(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // RECORDING -> STREAMING
    TEST_ASSERT_EQUAL(StateMachine::STATE_STREAMING, testStateMachine->getState());
}

// Test 5: BLE disconnection transitions STREAMING -> IDLE
void test_streaming_to_idle_on_ble_disconnection(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // RECORDING -> STREAMING
    testStateMachine->setBLEConnected(false);
    testStateMachine->run(); // STREAMING -> IDLE
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 6: Error state transitions
void test_error_state_transition(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->run();
    TEST_ASSERT_EQUAL(StateMachine::STATE_ERROR, testStateMachine->getState());
}

// Test 7: Error recovery after timeout
void test_error_recovery_after_timeout(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->setElapsedTime(5000); // 5 seconds
    testStateMachine->run();
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_ERROR, testStateMachine->getState());
    
    testStateMachine->setElapsedTime(10000); // 10 seconds (beyond recovery timeout)
    testStateMachine->run();
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 8: Recording duration tracking
void test_recording_duration_tracking(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setElapsedTime(5000); // 5 seconds
    testStateMachine->run();
    TEST_ASSERT_EQUAL(5000, testStateMachine->getRecordingDuration());
}

// Test 9: Streaming duration tracking
void test_streaming_duration_tracking(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setBLEConnected(true);
    testStateMachine->setElapsedTime(10000); // 10 seconds
    testStateMachine->run(); // RECORDING -> STREAMING
    TEST_ASSERT_EQUAL(10000, testStateMachine->getStreamingDuration());
}

// Test 10: Error duration tracking
void test_error_duration_tracking(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->setElapsedTime(5000); // 5 seconds
    testStateMachine->run();
    TEST_ASSERT_EQUAL(5000, testStateMachine->getErrorDuration());
}

// Test 11: Invalid state transition (button press in STREAMING)
void test_button_press_in_streaming(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // RECORDING -> STREAMING
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // Should stay in STREAMING
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_STREAMING, testStateMachine->getState());
}

// Test 12: BLE connection in IDLE state
void test_ble_connection_in_idle(void) {
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // Should stay in IDLE
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 13: Multiple button presses in RECORDING
void test_multiple_button_presses_in_recording(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // RECORDING -> IDLE
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // RECORDING -> IDLE
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 14: BLE disconnection in ERROR state
void test_ble_disconnection_in_error(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->setBLEConnected(false);
    testStateMachine->run(); // Should stay in ERROR
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_ERROR, testStateMachine->getState());
}

// Test 15: Error recovery timeout
void test_error_recovery_timeout(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->setElapsedTime(1000); // 1 second (below recovery timeout)
    testStateMachine->run();
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_ERROR, testStateMachine->getState());
    
    testStateMachine->setElapsedTime(10000); // 10 seconds (beyond recovery timeout)
    testStateMachine->run();
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 16: BLE reconnection in IDLE
void test_ble_reconnection_in_idle(void) {
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // Should stay in IDLE
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
    
    testStateMachine->setBLEConnected(false);
    testStateMachine->run(); // Should stay in IDLE
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 17: BLE connection in ERROR state
void test_ble_connection_in_error(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // Should stay in ERROR
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_ERROR, testStateMachine->getState());
}

// Test 18: Recording to streaming with BLE connection
void test_recording_to_streaming_with_ble(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // RECORDING -> STREAMING
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_STREAMING, testStateMachine->getState());
    TEST_ASSERT_TRUE(testStateMachine->isStreaming());
}

// Test 19: Streaming to recording with BLE disconnection
void test_streaming_to_recording_with_ble_disconnect(void) {
    testStateMachine->handleButtonPress(WatchStateMachine::BUTTON_PRESS); // IDLE -> RECORDING
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // RECORDING -> STREAMING
    testStateMachine->setBLEConnected(false);
    testStateMachine->run(); // STREAMING -> IDLE
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_IDLE, testStateMachine->getState());
}

// Test 20: Error state with BLE connection
void test_error_state_with_ble_connection(void) {
    testStateMachine->setError(WatchStateMachine::ERROR_AUDIO);
    testStateMachine->setBLEConnected(true);
    testStateMachine->run(); // Should stay in ERROR
    TEST_ASSERT_EQUAL(WatchStateMachine::STATE_ERROR, testStateMachine->getState());
}

// Run all tests
int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_idle_to_recording_on_button_press);
    RUN_TEST(test_recording_to_idle_on_button_press);
    RUN_TEST(test_recording_to_streaming_on_ble_connection);
    RUN_TEST(test_streaming_to_idle_on_ble_disconnection);
    RUN_TEST(test_error_state_transition);
    RUN_TEST(test_error_recovery_after_timeout);
    RUN_TEST(test_recording_duration_tracking);
    RUN_TEST(test_streaming_duration_tracking);
    RUN_TEST(test_error_duration_tracking);
    RUN_TEST(test_button_press_in_streaming);
    RUN_TEST(test_ble_connection_in_idle);
    RUN_TEST(test_multiple_button_presses_in_recording);
    RUN_TEST(test_ble_disconnection_in_error);
    RUN_TEST(test_error_recovery_timeout);
    RUN_TEST(test_ble_reconnection_in_idle);
    RUN_TEST(test_ble_connection_in_error);
    RUN_TEST(test_recording_to_streaming_with_ble);
    RUN_TEST(test_streaming_to_recording_with_ble_disconnect);
    RUN_TEST(test_error_state_with_ble_connection);
    UNITY_END();
    return 0;
}
