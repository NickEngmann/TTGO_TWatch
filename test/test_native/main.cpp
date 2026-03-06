#include "unity.h"
#include "WatchFace.h"

#ifdef NATIVE_BUILD
// Native build specific mock setup
void setup(void) {
    // Initialize mock state
    extern bool mock_bluetooth_connected;
    extern bool mock_recording;
    extern int8_t mock_battery_level;
    extern uint8_t mock_hours;
    extern uint8_t mock_minutes;
    extern uint8_t mock_seconds;
    extern uint8_t mock_day;
    extern uint8_t mock_month;
    extern uint16_t mock_year;
    
    mock_bluetooth_connected = false;
    mock_recording = false;
    mock_battery_level = -1;
    mock_hours = 12;
    mock_minutes = 30;
    mock_seconds = 0;
    mock_day = 1;
    mock_month = 1;
    mock_year = 2024;
}

void loop(void) {
    // Not used in native build
}
#endif

// Unity test runner main function
int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_WatchFace_Init_initializes_correctly);
    RUN_TEST(test_WatchFace_UpdateTime_updates_time_fields);
    RUN_TEST(test_WatchFace_UpdateTime_formats_time_correctly);
    RUN_TEST(test_WatchFace_UpdateTime_formats_date_correctly);
    RUN_TEST(test_WatchFace_UpdateBluetooth_updates_status);
    RUN_TEST(test_WatchFace_UpdateBluetooth_updates_label);
    RUN_TEST(test_WatchFace_UpdateRecording_updates_status);
    RUN_TEST(test_WatchFace_UpdateRecording_updates_label);
    RUN_TEST(test_WatchFace_UpdateBattery_updates_level);
    RUN_TEST(test_WatchFace_UpdateBattery_handles_negative);
    RUN_TEST(test_WatchFace_SetConfig_updates_configuration);
    RUN_TEST(test_WatchFace_GetState_returns_correct_state);
    RUN_TEST(test_WatchFace_Draw_triggers_refresh);
    RUN_TEST(test_WatchFace_UpdateTime_handles_midnight);
    RUN_TEST(test_WatchFace_UpdateTime_handles_single_digits);
    RUN_TEST(test_WatchFace_UpdateBluetooth_handles_state_transitions);
    RUN_TEST(test_WatchFace_UpdateRecording_handles_state_transitions);
    RUN_TEST(test_WatchFace_UpdateTime_updates_state_when_recording);
    RUN_TEST(test_WatchFace_UpdateTime_updates_state_when_bluetooth);
    RUN_TEST(test_WatchFace_Deinit_cleans_up);
    
#ifdef NATIVE_BUILD
    RUN_TEST(test_WatchFaceMock_GetBluetooth_initially_false);
    RUN_TEST(test_WatchFaceMock_SetBluetooth_updates_value);
    RUN_TEST(test_WatchFaceMock_GetRecording_initially_false);
    RUN_TEST(test_WatchFaceMock_SetRecording_updates_value);
    RUN_TEST(test_WatchFaceMock_GetBattery_initially_minus_one);
    RUN_TEST(test_WatchFaceMock_SetBattery_updates_value);
    RUN_TEST(test_WatchFaceMock_GetTime_values);
#endif
    
    UNITY_END();
    return 0;
}
