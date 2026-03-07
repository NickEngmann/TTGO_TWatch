/*
 * test_state_machine.c - Unity Tests for State Machine
 * 
 * Tests state transitions between IDLE, RECORDING, STREAMING, and SLEEP
 */

#include <unity.h>
#include <string.h>
#include <stdbool.h>
#include "StateMachine.h"

// Test state tracking variables
static AppState_t last_entered_state = STATE_INVALID;
static AppState_t last_exited_state = STATE_INVALID;
static uint32_t total_update_time = 0;

// Callback implementations for testing
static void TestStateEnterCallback(AppState_t state) {
    last_entered_state = state;
}

static void TestStateExitCallback(AppState_t state) {
    last_exited_state = state;
}

static void TestStateUpdateCallback(AppState_t state, uint32_t elapsed_ms) {
    total_update_time += elapsed_ms;
}



// Helper to create test config
static StateMachineConfig_t CreateTestConfig(void) {
    StateMachineConfig_t config = {
        .on_enter = TestStateEnterCallback,
        .on_exit = TestStateExitCallback,
        .on_update = TestStateUpdateCallback,
        .sleep_threshold_ms = 60000,  // 60 seconds
        .recording_buffer_size = 1024,
        .streaming_sample_rate = 44100
    };
    return config;
}

// Test 1: State Machine Initialization
void test_init_sets_idle_state(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_FALSE(StateMachine_IsRecording(&ctx));
    TEST_ASSERT_FALSE(StateMachine_IsStreaming(&ctx));
    TEST_ASSERT_FALSE(StateMachine_IsSleeping(&ctx));
    TEST_ASSERT_FALSE(StateMachine_IsBleConnected(&ctx));
}

// Test 2: Transition from IDLE to RECORDING via BLE connect
void test_idle_to_recording_ble_connect(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    
    TEST_ASSERT_EQUAL(STATE_RECORDING, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_TRUE(StateMachine_IsRecording(&ctx));
    TEST_ASSERT_EQUAL(STATE_IDLE, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_RECORDING, last_entered_state);
}

// Test 3: Transition from RECORDING to STREAMING
void test_recording_to_streaming(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    
    // Now transition to streaming
    StateMachine_ProcessEvent(&ctx, EVENT_START_STREAMING);
    
    TEST_ASSERT_EQUAL(STATE_STREAMING, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_TRUE(StateMachine_IsStreaming(&ctx));
    TEST_ASSERT_EQUAL(STATE_RECORDING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_STREAMING, last_entered_state);
}

// Test 4: Transition from STREAMING to IDLE
void test_streaming_to_idle(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    StateMachine_ProcessEvent(&ctx, EVENT_START_STREAMING);
    
    // Stop streaming
    StateMachine_ProcessEvent(&ctx, EVENT_STOP_STREAMING);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_TRUE(StateMachine_IsIdle(&ctx));
    TEST_ASSERT_EQUAL(STATE_STREAMING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 5: Transition from IDLE to SLEEP due to low battery
void test_idle_to_sleep_low_battery(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    
    // Trigger low battery event
    StateMachine_ProcessEvent(&ctx, EVENT_LOW_BATTERY);
    
    TEST_ASSERT_EQUAL(STATE_SLEEP, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_TRUE(StateMachine_IsSleeping(&ctx));
    TEST_ASSERT_EQUAL(STATE_IDLE, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_entered_state);
}

// Test 6: Transition from SLEEP to IDLE via wake up
void test_sleep_to_idle_wake_up(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_ProcessEvent(&ctx, EVENT_LOW_BATTERY);
    
    // Wake up from sleep
    StateMachine_ProcessEvent(&ctx, EVENT_WAKE_UP);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_TRUE(StateMachine_IsIdle(&ctx));
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 7: Transition from RECORDING to SLEEP due to low battery
void test_recording_to_sleep_low_battery(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    
    // Low battery during recording
    StateMachine_ProcessEvent(&ctx, EVENT_LOW_BATTERY);
    
    TEST_ASSERT_EQUAL(STATE_SLEEP, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_RECORDING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_entered_state);
}

// Test 8: Transition from STREAMING to SLEEP due to low battery
void test_streaming_to_sleep_low_battery(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    StateMachine_ProcessEvent(&ctx, EVENT_START_STREAMING);
    
    // Low battery during streaming
    StateMachine_ProcessEvent(&ctx, EVENT_LOW_BATTERY);
    
    TEST_ASSERT_EQUAL(STATE_SLEEP, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_STREAMING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_entered_state);
}

// Test 9: Sleep wake up via button press
void test_sleep_wake_button_press(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_ProcessEvent(&ctx, EVENT_LOW_BATTERY);
    
    // Button press wakes up device
    StateMachine_ProcessEvent(&ctx, EVENT_BUTTON_PRESSED);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 10: Idle to Sleep via idle timer
void test_idle_to_sleep_idle_timer(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    
    // Simulate idle time exceeding threshold
    StateMachine_Update(&ctx, 30000);  // 30 seconds
    StateMachine_Update(&ctx, 30000);  // 30 more seconds = 60 seconds total
    
    TEST_ASSERT_EQUAL(STATE_SLEEP, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_IDLE, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_entered_state);
}

// Test 11: Recording to Idle via stop recording (no BLE)
void test_recording_to_idle_stop(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    // Note: BLE is NOT connected, so stop recording goes to idle
    StateMachine_ProcessEvent(&ctx, EVENT_START_RECORDING);
    
    // Stop recording without BLE goes to idle
    StateMachine_ProcessEvent(&ctx, EVENT_STOP_RECORDING);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_RECORDING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 12: Recording to Streaming to Idle
void test_recording_to_streaming_to_idle(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    
    // Stop recording goes to streaming
    StateMachine_ProcessEvent(&ctx, EVENT_STOP_RECORDING);
    TEST_ASSERT_EQUAL(STATE_STREAMING, StateMachine_GetCurrentState(&ctx));
    
    // Stop streaming goes to idle
    StateMachine_ProcessEvent(&ctx, EVENT_STOP_STREAMING);
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_STREAMING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 13: BLE disconnect from recording
void test_recording_ble_disconnect(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    
    // BLE disconnect
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_DISCONNECTED);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_RECORDING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 14: BLE disconnect from streaming
void test_streaming_ble_disconnect(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_SetBleConnected(&ctx, true);
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_CONNECTED);
    StateMachine_ProcessEvent(&ctx, EVENT_START_STREAMING);
    
    // BLE disconnect
    StateMachine_ProcessEvent(&ctx, EVENT_BLE_DISCONNECTED);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_STREAMING, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 15: Update callback is called
void test_update_callback_called(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    // Reset the static counter before test
    total_update_time = 0;
    
    StateMachine_Init(&ctx, &config);
    
    // Update with 100ms
    StateMachine_Update(&ctx, 100);
    
    TEST_ASSERT_EQUAL(100, total_update_time);
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
}

// Test 16: Invalid state recovery
void test_invalid_state_recovery(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    
    // Set to invalid state directly (simulating corruption)
    ctx.current_state = STATE_INVALID;
    
    // Process event should recover to idle
    StateMachine_ProcessEvent(&ctx, EVENT_NONE);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
}

// Test 17: NULL context handling
void test_null_context_handling(void) {
    // These should not crash
    StateMachine_ProcessEvent(NULL, EVENT_NONE);
    StateMachine_Update(NULL, 100);
    
    TEST_ASSERT_EQUAL(STATE_INVALID, StateMachine_GetCurrentState(NULL));
    TEST_ASSERT_FALSE(StateMachine_IsRecording(NULL));
    TEST_ASSERT_FALSE(StateMachine_IsStreaming(NULL));
    TEST_ASSERT_FALSE(StateMachine_IsSleeping(NULL));
    TEST_ASSERT_FALSE(StateMachine_IsIdle(NULL));
    TEST_ASSERT_FALSE(StateMachine_IsBleConnected(NULL));
}

// Test 18: High battery event from sleep
void test_sleep_high_battery_wake(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_ProcessEvent(&ctx, EVENT_LOW_BATTERY);
    
    // High battery wakes up device
    StateMachine_ProcessEvent(&ctx, EVENT_HIGH_BATTERY);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

// Test 19: Manual sleep entry from idle
void test_manual_sleep_entry(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    
    // Manual sleep entry
    StateMachine_ProcessEvent(&ctx, EVENT_ENTER_SLEEP);
    
    TEST_ASSERT_EQUAL(STATE_SLEEP, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_IDLE, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_entered_state);
}

// Test 20: Manual wake up from sleep
void test_manual_wake_up(void) {
    StateMachineContext_t ctx;
    StateMachineConfig_t config = CreateTestConfig();
    
    StateMachine_Init(&ctx, &config);
    StateMachine_ProcessEvent(&ctx, EVENT_ENTER_SLEEP);
    
    // Manual wake up
    StateMachine_ProcessEvent(&ctx, EVENT_WAKE_UP);
    
    TEST_ASSERT_EQUAL(STATE_IDLE, StateMachine_GetCurrentState(&ctx));
    TEST_ASSERT_EQUAL(STATE_SLEEP, last_exited_state);
    TEST_ASSERT_EQUAL(STATE_IDLE, last_entered_state);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_init_sets_idle_state);
    RUN_TEST(test_idle_to_recording_ble_connect);
    RUN_TEST(test_recording_to_streaming);
    RUN_TEST(test_streaming_to_idle);
    RUN_TEST(test_idle_to_sleep_low_battery);
    RUN_TEST(test_sleep_to_idle_wake_up);
    RUN_TEST(test_recording_to_sleep_low_battery);
    RUN_TEST(test_streaming_to_sleep_low_battery);
    RUN_TEST(test_sleep_wake_button_press);
    RUN_TEST(test_idle_to_sleep_idle_timer);
    RUN_TEST(test_recording_to_idle_stop);
    RUN_TEST(test_recording_to_streaming_to_idle);
    RUN_TEST(test_recording_ble_disconnect);
    RUN_TEST(test_streaming_ble_disconnect);
    RUN_TEST(test_update_callback_called);
    RUN_TEST(test_invalid_state_recovery);
    RUN_TEST(test_null_context_handling);
    RUN_TEST(test_sleep_high_battery_wake);
    RUN_TEST(test_manual_sleep_entry);
    RUN_TEST(test_manual_wake_up);
    
    return UNITY_END();
}
