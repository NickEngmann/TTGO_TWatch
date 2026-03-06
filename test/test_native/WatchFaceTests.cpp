#include "unity.h"
#include "WatchFace.h"
#include <stdio.h>
#include <string.h>

// Mock LVGL functions for native testing
static char mock_time_label_text[64];
static char mock_date_label_text[64];
static char mock_bluetooth_label_text[64];
static char mock_recording_label_text[16];
static char mock_battery_label_text[16];

static void mock_lv_label_set_text(lv_obj_t *obj, const char *text) {
    if (obj == (lv_obj_t*)1) {
        strncpy(mock_time_label_text, text, sizeof(mock_time_label_text) - 1);
        mock_time_label_text[sizeof(mock_time_label_text) - 1] = '\0';
    } else if (obj == (lv_obj_t*)2) {
        strncpy(mock_date_label_text, text, sizeof(mock_date_label_text) - 1);
        mock_date_label_text[sizeof(mock_date_label_text) - 1] = '\0';
    } else if (obj == (lv_obj_t*)4) {
        strncpy(mock_bluetooth_label_text, text, sizeof(mock_bluetooth_label_text) - 1);
        mock_bluetooth_label_text[sizeof(mock_bluetooth_label_text) - 1] = '\0';
    } else if (obj == (lv_obj_t*)6) {
        strncpy(mock_recording_label_text, text, sizeof(mock_recording_label_text) - 1);
        mock_recording_label_text[sizeof(mock_recording_label_text) - 1] = '\0';
    } else if (obj == (lv_obj_t*)8) {
        strncpy(mock_battery_label_text, text, sizeof(mock_battery_label_text) - 1);
        mock_battery_label_text[sizeof(mock_battery_label_text) - 1] = '\0';
    }
}

// Test setup
typedef struct {
    WatchFace_t watchFace;
    lv_obj_t timeLabel;
    lv_obj_t dateLabel;
    lv_obj_t bluetoothIcon;
    lv_obj_t bluetoothLabel;
    lv_obj_t recordingIcon;
    lv_obj_t recordingLabel;
    lv_obj_t batteryIcon;
    lv_obj_t batteryLabel;
} TestContext_t;

static TestContext_t testContext;

void setUp(void) {
    memset(&testContext, 0, sizeof(TestContext_t));
    
    // Initialize mock LVGL objects with unique addresses for testing
    testContext.timeLabel = (lv_obj_t){0};
    testContext.dateLabel = (lv_obj_t){1};
    testContext.bluetoothIcon = (lv_obj_t){2};
    testContext.bluetoothLabel = (lv_obj_t){3};
    testContext.recordingIcon = (lv_obj_t){4};
    testContext.recordingLabel = (lv_obj_t){5};
    testContext.batteryIcon = (lv_obj_t){6};
    testContext.batteryLabel = (lv_obj_t){7};
    
    // Initialize watch face
    WatchFace_Init(&testContext.watchFace);
    
    // Assign mock objects to watch face
    testContext.watchFace.timeLabel = &testContext.timeLabel;
    testContext.watchFace.dateLabel = &testContext.dateLabel;
    testContext.watchFace.bluetoothIcon = &testContext.bluetoothIcon;
    testContext.watchFace.bluetoothLabel = &testContext.bluetoothLabel;
    testContext.watchFace.recordingIcon = &testContext.recordingIcon;
    testContext.watchFace.recordingLabel = &testContext.recordingLabel;
    testContext.watchFace.batteryIcon = &testContext.batteryIcon;
    testContext.watchFace.batteryLabel = &testContext.batteryLabel;
    
    // Clear mock text buffers
    mock_time_label_text[0] = '\0';
    mock_date_label_text[0] = '\0';
    mock_bluetooth_label_text[0] = '\0';
    mock_recording_label_text[0] = '\0';
    mock_battery_label_text[0] = '\0';
}

void tearDown(void) {
    WatchFace_Deinit(&testContext.watchFace);
}

// Test 1: WatchFace_Init should initialize all fields correctly
void test_WatchFace_Init_initializes_correctly(void) {
    TEST_ASSERT_NOT_NULL(testContext.watchFace.timeLabel);
    TEST_ASSERT_NOT_NULL(testContext.watchFace.dateLabel);
    TEST_ASSERT_EQUAL(WATCH_STATE_NORMAL, testContext.watchFace.context.state);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showSeconds);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showDate);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showBattery);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showBluetooth);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showRecording);
    TEST_ASSERT_EQUAL_INT8(-1, testContext.watchFace.context.batteryLevel);
    TEST_ASSERT_FALSE(testContext.watchFace.context.isBluetoothConnected);
    TEST_ASSERT_FALSE(testContext.watchFace.context.isRecording);
}

// Test 2: WatchFace_UpdateTime should update time fields correctly
void test_WatchFace_UpdateTime_updates_time_fields(void) {
    WatchFace_UpdateTime(&testContext.watchFace, 14, 30, 45, 15, 6, 2024);
    
    TEST_ASSERT_EQUAL_UINT8(14, testContext.watchFace.context.hours);
    TEST_ASSERT_EQUAL_UINT8(30, testContext.watchFace.context.minutes);
    TEST_ASSERT_EQUAL_UINT8(45, testContext.watchFace.context.seconds);
    TEST_ASSERT_EQUAL_UINT8(15, testContext.watchFace.context.day);
    TEST_ASSERT_EQUAL_UINT8(6, testContext.watchFace.context.month);
    TEST_ASSERT_EQUAL_UINT16(2024, testContext.watchFace.context.year);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_NORMAL, testContext.watchFace.context.state);
}

// Test 3: WatchFace_UpdateTime should format time correctly
void test_WatchFace_UpdateTime_formats_time_correctly(void) {
    WatchFace_UpdateTime(&testContext.watchFace, 9, 5, 3, 1, 1, 2024);
    
    // Time should be formatted as HH:MM:SS
    TEST_ASSERT_EQUAL_STRING("09:05:03", mock_time_label_text);
}

// Test 4: WatchFace_UpdateTime should format date correctly
void test_WatchFace_UpdateTime_formats_date_correctly(void) {
    WatchFace_UpdateTime(&testContext.watchFace, 12, 30, 0, 25, 12, 2023);
    
    // Date should be formatted as DD/MM/YYYY
    TEST_ASSERT_EQUAL_STRING("25/12/2023", mock_date_label_text);
}

// Test 5: WatchFace_UpdateBluetooth should update Bluetooth status
void test_WatchFace_UpdateBluetooth_updates_status(void) {
    WatchFace_UpdateBluetooth(&testContext.watchFace, true);
    
    TEST_ASSERT_TRUE(testContext.watchFace.context.isBluetoothConnected);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_BLE_CONNECTED, testContext.watchFace.context.state);
}

// Test 6: WatchFace_UpdateBluetooth should update Bluetooth label text
void test_WatchFace_UpdateBluetooth_updates_label(void) {
    WatchFace_UpdateBluetooth(&testContext.watchFace, true);
    TEST_ASSERT_EQUAL_STRING("Connected", mock_bluetooth_label_text);
    
    WatchFace_UpdateBluetooth(&testContext.watchFace, false);
    TEST_ASSERT_EQUAL_STRING("Disconnected", mock_bluetooth_label_text);
}

// Test 7: WatchFace_UpdateRecording should update recording status
void test_WatchFace_UpdateRecording_updates_status(void) {
    WatchFace_UpdateRecording(&testContext.watchFace, true);
    
    TEST_ASSERT_TRUE(testContext.watchFace.context.isRecording);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_RECORDING, testContext.watchFace.context.state);
}

// Test 8: WatchFace_UpdateRecording should update recording label text
void test_WatchFace_UpdateRecording_updates_label(void) {
    WatchFace_UpdateRecording(&testContext.watchFace, true);
    TEST_ASSERT_EQUAL_STRING("REC", mock_recording_label_text);
    
    WatchFace_UpdateRecording(&testContext.watchFace, false);
    TEST_ASSERT_EQUAL_STRING("", mock_recording_label_text);
}

// Test 9: WatchFace_UpdateBattery should update battery level
void test_WatchFace_UpdateBattery_updates_level(void) {
    WatchFace_UpdateBattery(&testContext.watchFace, 75);
    
    TEST_ASSERT_EQUAL_INT8(75, testContext.watchFace.context.batteryLevel);
    TEST_ASSERT_EQUAL_STRING("75%", mock_battery_label_text);
}

// Test 10: WatchFace_UpdateBattery should handle negative battery level
void test_WatchFace_UpdateBattery_handles_negative(void) {
    WatchFace_UpdateBattery(&testContext.watchFace, -1);
    
    TEST_ASSERT_EQUAL_INT8(-1, testContext.watchFace.context.batteryLevel);
    TEST_ASSERT_EQUAL_STRING("--", mock_battery_label_text);
}

// Test 11: WatchFace_SetConfig should update configuration
void test_WatchFace_SetConfig_updates_configuration(void) {
    WatchFaceConfig_t config = {
        .showSeconds = false,
        .showDate = true,
        .showBattery = true,
        .showBluetooth = false,
        .showRecording = true
    };
    
    WatchFace_SetConfig(&testContext.watchFace, &config);
    
    TEST_ASSERT_FALSE(testContext.watchFace.context.config.showSeconds);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showDate);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showBattery);
    TEST_ASSERT_FALSE(testContext.watchFace.context.config.showBluetooth);
    TEST_ASSERT_TRUE(testContext.watchFace.context.config.showRecording);
}

// Test 12: WatchFace_GetState should return correct state
void test_WatchFace_GetState_returns_correct_state(void) {
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_NORMAL, WatchFace_GetState(&testContext.watchFace));
    
    WatchFace_UpdateRecording(&testContext.watchFace, true);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_RECORDING, WatchFace_GetState(&testContext.watchFace));
    
    WatchFace_UpdateRecording(&testContext.watchFace, false);
    WatchFace_UpdateBluetooth(&testContext.watchFace, true);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_BLE_CONNECTED, WatchFace_GetState(&testContext.watchFace));
}

// Test 13: WatchFace_Draw should trigger LVGL refresh
void test_WatchFace_Draw_triggers_refresh(void) {
    // This test verifies the Draw function exists and can be called
    WatchFace_Draw(&testContext.watchFace);
    TEST_PASS();
}

// Test 14: WatchFace_UpdateTime should handle midnight correctly
void test_WatchFace_UpdateTime_handles_midnight(void) {
    WatchFace_UpdateTime(&testContext.watchFace, 23, 59, 59, 31, 12, 2023);
    
    TEST_ASSERT_EQUAL_STRING("23:59:59", mock_time_label_text);
    TEST_ASSERT_EQUAL_STRING("31/12/2023", mock_date_label_text);
}

// Test 15: WatchFace_UpdateTime should handle single digit values
void test_WatchFace_UpdateTime_handles_single_digits(void) {
    WatchFace_UpdateTime(&testContext.watchFace, 0, 0, 0, 1, 1, 2024);
    
    TEST_ASSERT_EQUAL_STRING("00:00:00", mock_time_label_text);
    TEST_ASSERT_EQUAL_STRING("01/01/2024", mock_date_label_text);
}

// Test 16: WatchFace_UpdateBluetooth should handle state transitions
void test_WatchFace_UpdateBluetooth_handles_state_transitions(void) {
    WatchFace_UpdateBluetooth(&testContext.watchFace, true);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_BLE_CONNECTED, testContext.watchFace.context.state);
    
    WatchFace_UpdateBluetooth(&testContext.watchFace, false);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_BLE_DISCONNECTED, testContext.watchFace.context.state);
}

// Test 17: WatchFace_UpdateRecording should handle state transitions
void test_WatchFace_UpdateRecording_handles_state_transitions(void) {
    WatchFace_UpdateRecording(&testContext.watchFace, true);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_RECORDING, testContext.watchFace.context.state);
    
    WatchFace_UpdateRecording(&testContext.watchFace, false);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_NORMAL, testContext.watchFace.context.state);
}

// Test 18: WatchFace_UpdateTime should update state when recording
void test_WatchFace_UpdateTime_updates_state_when_recording(void) {
    WatchFace_UpdateRecording(&testContext.watchFace, true);
    WatchFace_UpdateTime(&testContext.watchFace, 12, 0, 0, 1, 1, 2024);
    
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_RECORDING, testContext.watchFace.context.state);
}

// Test 19: WatchFace_UpdateTime should update state when Bluetooth connected
void test_WatchFace_UpdateTime_updates_state_when_bluetooth(void) {
    WatchFace_UpdateBluetooth(&testContext.watchFace, true);
    WatchFace_UpdateTime(&testContext.watchFace, 12, 0, 0, 1, 1, 2024);
    
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_BLE_CONNECTED, testContext.watchFace.context.state);
}

// Test 20: WatchFace_Deinit should clean up resources
void test_WatchFace_Deinit_cleans_up(void) {
    WatchFace_Deinit(&testContext.watchFace);
    TEST_ASSERT_EQUAL_INT(WATCH_STATE_NORMAL, testContext.watchFace.context.state);
}

#ifdef NATIVE_BUILD
// Native build specific tests
void test_WatchFaceMock_GetBluetooth_initially_false(void) {
    TEST_ASSERT_FALSE(WatchFaceMock_GetBluetooth());
}

void test_WatchFaceMock_SetBluetooth_updates_value(void) {
    WatchFaceMock_SetBluetooth(true);
    TEST_ASSERT_TRUE(WatchFaceMock_GetBluetooth());
    
    WatchFaceMock_SetBluetooth(false);
    TEST_ASSERT_FALSE(WatchFaceMock_GetBluetooth());
}

void test_WatchFaceMock_GetRecording_initially_false(void) {
    TEST_ASSERT_FALSE(WatchFaceMock_GetRecording());
}

void test_WatchFaceMock_SetRecording_updates_value(void) {
    WatchFaceMock_SetRecording(true);
    TEST_ASSERT_TRUE(WatchFaceMock_GetRecording());
    
    WatchFaceMock_SetRecording(false);
    TEST_ASSERT_FALSE(WatchFaceMock_GetRecording());
}

void test_WatchFaceMock_GetBattery_initially_minus_one(void) {
    TEST_ASSERT_EQUAL_INT8(-1, WatchFaceMock_GetBattery());
}

void test_WatchFaceMock_SetBattery_updates_value(void) {
    WatchFaceMock_SetBattery(100);
    TEST_ASSERT_EQUAL_INT8(100, WatchFaceMock_GetBattery());
    
    WatchFaceMock_SetBattery(50);
    TEST_ASSERT_EQUAL_INT8(50, WatchFaceMock_GetBattery());
}

void test_WatchFaceMock_GetTime_values(void) {
    WatchFaceMock_SetTime(15, 45, 30, 20, 7, 2024);
    TEST_ASSERT_EQUAL_UINT8(15, WatchFaceMock_GetHours());
    TEST_ASSERT_EQUAL_UINT8(45, WatchFaceMock_GetMinutes());
    TEST_ASSERT_EQUAL_UINT8(30, WatchFaceMock_GetSeconds());
    TEST_ASSERT_EQUAL_UINT8(20, WatchFaceMock_GetDay());
    TEST_ASSERT_EQUAL_UINT8(7, WatchFaceMock_GetMonth());
    TEST_ASSERT_EQUAL_UINT16(2024, WatchFaceMock_GetYear());
}
#endif
