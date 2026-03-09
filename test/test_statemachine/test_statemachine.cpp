#include <unity.h>
#include "../../src/StateMachine.cpp"

static StateMachine* sm = nullptr;
static int stateChangeCount = 0;
static StateMachine::State lastOldState;
static StateMachine::State lastNewState;

static void stateChangeCallback(StateMachine::State oldState, StateMachine::State newState) {
    stateChangeCount++;
    lastOldState = oldState;
    lastNewState = newState;
}

void setUp(void) {
    sm = new StateMachine();
    sm->init();
    stateChangeCount = 0;
}

void tearDown(void) {
    delete sm;
    sm = nullptr;
}

void test_initial_state_is_idle(void) {
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_button_press_idle_to_recording(void) {
    sm->onButtonPress();
    TEST_ASSERT_EQUAL(StateMachine::RECORDING, sm->getCurrentState());
}

void test_button_press_recording_to_idle(void) {
    sm->onButtonPress();  // IDLE -> RECORDING
    sm->onButtonPress();  // RECORDING -> IDLE
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_ble_connect_recording_to_streaming(void) {
    sm->onButtonPress();  // IDLE -> RECORDING
    sm->onBLEConnect();
    TEST_ASSERT_EQUAL(StateMachine::STREAMING, sm->getCurrentState());
}

void test_ble_disconnect_streaming_to_recording(void) {
    sm->onButtonPress();
    sm->onBLEConnect();
    TEST_ASSERT_EQUAL(StateMachine::STREAMING, sm->getCurrentState());
    sm->onBLEDisconnect();
    TEST_ASSERT_EQUAL(StateMachine::RECORDING, sm->getCurrentState());
}

void test_idle_timeout_to_deep_sleep(void) {
    sm->update(31000);  // > 30s default timeout
    TEST_ASSERT_EQUAL(StateMachine::DEEP_SLEEP, sm->getCurrentState());
}

void test_error_auto_recovery(void) {
    sm->onError();
    TEST_ASSERT_EQUAL(StateMachine::ERROR, sm->getCurrentState());
    sm->update(6000);  // > 5s recovery time
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_watchdog_timeout(void) {
    sm->onButtonPress();  // RECORDING
    sm->update(61000);  // > 60s watchdog
    TEST_ASSERT_EQUAL(StateMachine::ERROR, sm->getCurrentState());
}

void test_state_change_callback(void) {
    sm->setStateChangeCallback(stateChangeCallback);
    sm->onButtonPress();
    TEST_ASSERT_EQUAL(1, stateChangeCount);
    TEST_ASSERT_EQUAL(StateMachine::IDLE, lastOldState);
    TEST_ASSERT_EQUAL(StateMachine::RECORDING, lastNewState);
}

void test_button_wakes_from_deep_sleep(void) {
    sm->update(31000);  // Force deep sleep
    TEST_ASSERT_EQUAL(StateMachine::DEEP_SLEEP, sm->getCurrentState());
    sm->onButtonPress();
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_button_recovers_from_error(void) {
    sm->onError();
    sm->onButtonPress();
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_ble_connect_in_idle_no_transition(void) {
    sm->onBLEConnect();
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_multiple_button_toggles(void) {
    sm->onButtonPress();  // IDLE -> RECORDING
    sm->onButtonPress();  // RECORDING -> IDLE
    sm->onButtonPress();  // IDLE -> RECORDING
    TEST_ASSERT_EQUAL(StateMachine::RECORDING, sm->getCurrentState());
}

void test_ble_reconnect_cycle(void) {
    sm->onButtonPress();  // RECORDING
    sm->onBLEConnect();   // STREAMING
    sm->onBLEDisconnect(); // RECORDING
    sm->onBLEConnect();   // STREAMING
    TEST_ASSERT_EQUAL(StateMachine::STREAMING, sm->getCurrentState());
}

void test_configurable_idle_timeout(void) {
    StateMachine::Config cfg;
    cfg.idleTimeoutMs = 10000;
    sm->setConfig(cfg);
    sm->update(11000);
    TEST_ASSERT_EQUAL(StateMachine::DEEP_SLEEP, sm->getCurrentState());
}

void test_configurable_error_recovery(void) {
    StateMachine::Config cfg;
    cfg.errorRecoveryMs = 2000;
    sm->setConfig(cfg);
    sm->onError();
    sm->update(3000);
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

void test_state_name(void) {
    TEST_ASSERT_EQUAL_STRING("IDLE", sm->getStateName());
    sm->onButtonPress();
    TEST_ASSERT_EQUAL_STRING("RECORDING", sm->getStateName());
}

void test_time_in_state(void) {
    TEST_ASSERT_EQUAL_UINT32(5000, sm->getTimeInState(5000));
}

void test_request_recording(void) {
    sm->requestRecording();
    TEST_ASSERT_EQUAL(StateMachine::RECORDING, sm->getCurrentState());
}

void test_request_idle_from_recording(void) {
    sm->onButtonPress();  // RECORDING
    sm->requestIdle();
    TEST_ASSERT_EQUAL(StateMachine::IDLE, sm->getCurrentState());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_button_press_idle_to_recording);
    RUN_TEST(test_button_press_recording_to_idle);
    RUN_TEST(test_ble_connect_recording_to_streaming);
    RUN_TEST(test_ble_disconnect_streaming_to_recording);
    RUN_TEST(test_idle_timeout_to_deep_sleep);
    RUN_TEST(test_error_auto_recovery);
    RUN_TEST(test_watchdog_timeout);
    RUN_TEST(test_state_change_callback);
    RUN_TEST(test_button_wakes_from_deep_sleep);
    RUN_TEST(test_button_recovers_from_error);
    RUN_TEST(test_ble_connect_in_idle_no_transition);
    RUN_TEST(test_multiple_button_toggles);
    RUN_TEST(test_ble_reconnect_cycle);
    RUN_TEST(test_configurable_idle_timeout);
    RUN_TEST(test_configurable_error_recovery);
    RUN_TEST(test_state_name);
    RUN_TEST(test_time_in_state);
    RUN_TEST(test_request_recording);
    RUN_TEST(test_request_idle_from_recording);
    return UNITY_END();
}
