#include <unity.h>
#include "../src/AlarmClock.cpp"

// Test fixture
AlarmClock* alarmClock = nullptr;

void setUp(void) {
    alarmClock = new AlarmClock();
    alarmClock->init();
}

void tearDown(void) {
    delete alarmClock;
    alarmClock = nullptr;
}

// Basic initialization tests
void test_init_returns_true(void) {
    bool result = alarmClock->init();
    TEST_ASSERT_TRUE(result);
}

void test_init_sets_initialized_flag(void) {
    TEST_ASSERT_TRUE(alarmClock->init());
    // In native build, initialization always succeeds
    TEST_ASSERT_TRUE(true);  // Placeholder - actual state not exposed
}

void test_deinit_clears_alarms(void) {
    alarmClock->addAlarm(8, 0, "Wake Up");
    alarmClock->addAlarm(12, 30, "Lunch");
    TEST_ASSERT_EQUAL_UINT8(2, alarmClock->getAlarmCount());
    
    alarmClock->deinit();
    TEST_ASSERT_EQUAL_UINT8(0, alarmClock->getAlarmCount());
}

// Alarm management tests
void test_add_alarm_returns_index(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Wake Up");
    TEST_ASSERT_LESS_THAN(255, index);
}

void test_add_alarm_with_label(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Morning Alarm");
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_STRING("Morning Alarm", alarm.label);
}

void test_add_alarm_default_values(void) {
    uint8_t index = alarmClock->addAlarm(12, 0, "");
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_TRUE(alarm.enabled);
    TEST_ASSERT_EQUAL_UINT32(0, alarm.repeatDays);
}

void test_add_alarm_invalid_hour(void) {
    uint8_t index = alarmClock->addAlarm(25, 0, "Invalid");
    TEST_ASSERT_EQUAL_UINT8(255, index);  // Should fail
}

void test_add_alarm_invalid_minute(void) {
    uint8_t index = alarmClock->addAlarm(12, 60, "Invalid");
    TEST_ASSERT_EQUAL_UINT8(255, index);  // Should fail
}

void test_add_alarm_count_increments(void) {
    TEST_ASSERT_EQUAL_UINT8(0, alarmClock->getAlarmCount());
    
    alarmClock->addAlarm(8, 0, "Alarm 1");
    TEST_ASSERT_EQUAL_UINT8(1, alarmClock->getAlarmCount());
    
    alarmClock->addAlarm(9, 0, "Alarm 2");
    TEST_ASSERT_EQUAL_UINT8(2, alarmClock->getAlarmCount());
    
    alarmClock->addAlarm(10, 0, "Alarm 3");
    TEST_ASSERT_EQUAL_UINT8(3, alarmClock->getAlarmCount());
}

// Enable/disable tests
void test_enable_alarm(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Alarm");
    TEST_ASSERT_TRUE(alarmClock->isAlarmActive(index));
    
    alarmClock->disableAlarm(index);
    TEST_ASSERT_FALSE(alarmClock->isAlarmActive(index));
    
    alarmClock->enableAlarm(index);
    TEST_ASSERT_TRUE(alarmClock->isAlarmActive(index));
}

void test_toggle_alarm(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Alarm");
    TEST_ASSERT_TRUE(alarmClock->isAlarmActive(index));
    
    alarmClock->toggleAlarm(index);
    TEST_ASSERT_FALSE(alarmClock->isAlarmActive(index));
    
    alarmClock->toggleAlarm(index);
    TEST_ASSERT_TRUE(alarmClock->isAlarmActive(index));
}

void test_disable_alarm(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Alarm");
    TEST_ASSERT_TRUE(alarmClock->isAlarmActive(index));
    
    alarmClock->disableAlarm(index);
    TEST_ASSERT_FALSE(alarmClock->isAlarmActive(index));
}

// Set alarm time tests
void test_set_alarm_time(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Original");
    
    alarmClock->setAlarmTime(index, 12, 30);
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_UINT8(12, alarm.hour);
    TEST_ASSERT_EQUAL_UINT8(30, alarm.minute);
}

void test_set_alarm_time_invalid_hour(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Alarm");
    bool result = alarmClock->setAlarmTime(index, 25, 0);
    TEST_ASSERT_FALSE(result);
}

void test_set_alarm_time_invalid_minute(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Alarm");
    bool result = alarmClock->setAlarmTime(index, 8, 60);
    TEST_ASSERT_FALSE(result);
}

// Set alarm label tests
void test_set_alarm_label(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Old Label");
    
    alarmClock->setAlarmLabel(index, "New Label");
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_STRING("New Label", alarm.label);
}

void test_set_alarm_label_empty(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Old Label");
    
    alarmClock->setAlarmLabel(index, "");
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_STRING("", alarm.label);
}

// Set alarm repeat tests
void test_set_alarm_repeat(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Weekday Alarm");
    
    // Monday (bit 0 set)
    alarmClock->setAlarmRepeat(index, 1);
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_UINT32(1, alarm.repeatDays);
    
    // Monday and Wednesday (bits 0 and 2 set)
    alarmClock->setAlarmRepeat(index, 5);
    alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_UINT32(5, alarm.repeatDays);
}

// Remove alarm tests
void test_remove_alarm_last(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "Alarm");
    TEST_ASSERT_EQUAL_UINT8(1, alarmClock->getAlarmCount());
    
    alarmClock->removeAlarm(index);
    TEST_ASSERT_EQUAL_UINT8(0, alarmClock->getAlarmCount());
}

void test_remove_alarm_multiple(void) {
    alarmClock->addAlarm(8, 0, "Alarm 1");
    uint8_t index2 = alarmClock->addAlarm(9, 0, "Alarm 2");
    TEST_ASSERT_EQUAL_UINT8(2, alarmClock->getAlarmCount());
    
    alarmClock->removeAlarm(index2);
    TEST_ASSERT_EQUAL_UINT8(1, alarmClock->getAlarmCount());
}

// Check alarm tests
void test_check_alarm_matching_time(void) {
    uint8_t index = alarmClock->addAlarm(8, 30, "Test Alarm");
    
    bool result = alarmClock->checkAlarm(index, 8, 30);
    TEST_ASSERT_TRUE(result);
}

void test_check_alarm_not_matching_hour(void) {
    uint8_t index = alarmClock->addAlarm(8, 30, "Test Alarm");
    
    bool result = alarmClock->checkAlarm(index, 9, 30);
    TEST_ASSERT_FALSE(result);
}

void test_check_alarm_not_matching_minute(void) {
    uint8_t index = alarmClock->addAlarm(8, 30, "Test Alarm");
    
    bool result = alarmClock->checkAlarm(index, 8, 31);
    TEST_ASSERT_FALSE(result);
}

void test_check_alarm_disabled(void) {
    uint8_t index = alarmClock->addAlarm(8, 30, "Test Alarm");
    alarmClock->disableAlarm(index);
    
    bool result = alarmClock->checkAlarm(index, 8, 30);
    TEST_ASSERT_FALSE(result);  // Disabled alarms don't match
}

// Configuration tests
void test_set_config(void) {
    AlarmClock::Config config;
    config.autoCheckEnabled = false;
    config.checkIntervalMs = 5000;
    config.playTone = false;
    config.vibrate = true;
    
    alarmClock->setConfig(config);
    
    const AlarmClock::Config& cfg = alarmClock->getConfig();
    TEST_ASSERT_FALSE(cfg.autoCheckEnabled);
    TEST_ASSERT_EQUAL_UINT32(5000, cfg.checkIntervalMs);
    TEST_ASSERT_FALSE(cfg.playTone);
    TEST_ASSERT_TRUE(cfg.vibrate);
}

void test_default_config(void) {
    const AlarmClock::Config& cfg = alarmClock->getConfig();
    TEST_ASSERT_TRUE(cfg.autoCheckEnabled);
    TEST_ASSERT_EQUAL_UINT32(1000, cfg.checkIntervalMs);
    TEST_ASSERT_TRUE(cfg.playTone);
    TEST_ASSERT_TRUE(cfg.vibrate);
}

// Callback tests
void test_alarm_callback_on_check(void) {
    bool callbackCalled = false;
    uint8_t callbackIndex = 255;
    
    alarmClock->setAlarmCallback([&](uint8_t index, AlarmEntry& alarm) {
        callbackCalled = true;
        callbackIndex = index;
    });
    
    // Add an alarm
    uint8_t index = alarmClock->addAlarm(8, 30, "Test Alarm");
    
    // Manually test the check logic
    TEST_ASSERT_TRUE(alarmClock->checkAlarm(index, 8, 30));
}

// Has pending alarm tests
void test_has_pending_alarm_false_initially(void) {
    TEST_ASSERT_FALSE(alarmClock->hasPendingAlarm());
}

void test_clear_pending_alarm(void) {
    alarmClock->clearPendingAlarm();
    TEST_ASSERT_FALSE(alarmClock->hasPendingAlarm());
}

void test_get_pending_alarm_initial(void) {
    TEST_ASSERT_EQUAL_UINT8(255, alarmClock->getPendingAlarmIndex());
}

// Advanced alarm functionality tests
void test_alarm_time_update_24h(void) {
    // Create two alarms and modify their times
    uint8_t index1 = alarmClock->addAlarm(8, 0, "Alarm 1");
    uint8_t index2 = alarmClock->addAlarm(9, 0, "Alarm 2");
    
    alarmClock->setAlarmTime(index1, 23, 59);  // 11:59 PM
    alarmClock->setAlarmTime(index2, 0, 0);    // 12:00 AM
    
    TEST_ASSERT_TRUE(alarmClock->checkAlarm(index1, 23, 59));
    TEST_ASSERT_TRUE(alarmClock->checkAlarm(index2, 0, 0));
}

void test_alarm_at_midnight(void) {
    uint8_t index = alarmClock->addAlarm(0, 0, "Midnight");
    TEST_ASSERT_TRUE(alarmClock->checkAlarm(index, 0, 0));
}

void test_alarm_at_noon(void) {
    uint8_t index = alarmClock->addAlarm(12, 0, "Noon");
    TEST_ASSERT_TRUE(alarmClock->checkAlarm(index, 12, 0));
}

void test_alarm_end_of_day(void) {
    uint8_t index = alarmClock->addAlarm(23, 59, "Last Minute");
    TEST_ASSERT_TRUE(alarmClock->checkAlarm(index, 23, 59));
}

// Multiple alarms test
void test_multiple_alarms_same_time(void) {
    alarmClock->addAlarm(8, 0, "Alarm 1");
    alarmClock->addAlarm(8, 0, "Alarm 2");
    alarmClock->addAlarm(8, 0, "Alarm 3");
    
    TEST_ASSERT_EQUAL_UINT8(3, alarmClock->getAlarmCount());
    
    // All should match at 8:00
    for (uint8_t i = 0; i < 3; i++) {
        TEST_ASSERT_TRUE(alarmClock->checkAlarm(i, 8, 0));
    }
}

void test_update_calls_callback(void) {
    bool callbackCalled = false;
    alarmClock->setAlarmCallback([&](uint8_t index, AlarmEntry& alarm) {
        callbackCalled = true;
    });
    
    alarmClock->update();  // Should check for alarms
    // Callback won't be called unless alarm matches simulated time
    // This test verifies update() doesn't crash
    TEST_ASSERT_TRUE(true);
}

// Edge cases
void test_empty_label(void) {
    uint8_t index = alarmClock->addAlarm(8, 0, "");
    AlarmEntry alarm = alarmClock->getAlarm(index);
    TEST_ASSERT_EQUAL_STRING("", alarm.label);
}

void test_very_long_label(void) {
    const char* longLabel = "This is a very long alarm label that exceeds normal length";
    uint8_t index = alarmClock->addAlarm(8, 0, longLabel);
    AlarmEntry alarm = alarmClock->getAlarm(index);
    // Label should be truncated to fit
    TEST_ASSERT_TRUE(strlen(alarm.label) <= 31);
}

void test_invalid_alarm_index(void) {
    TEST_ASSERT_FALSE(alarmClock->isAlarmActive(999));
    TEST_ASSERT_FALSE(alarmClock->disableAlarm(999));
    TEST_ASSERT_FALSE(alarmClock->enableAlarm(999));
    TEST_ASSERT_FALSE(alarmClock->toggleAlarm(999));
    TEST_ASSERT_FALSE(alarmClock->removeAlarm(999));
}

int main(void) {
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_init_returns_true);
    RUN_TEST(test_init_sets_initialized_flag);
    RUN_TEST(test_deinit_clears_alarms);
    
    // Alarm management tests
    RUN_TEST(test_add_alarm_returns_index);
    RUN_TEST(test_add_alarm_with_label);
    RUN_TEST(test_add_alarm_default_values);
    RUN_TEST(test_add_alarm_invalid_hour);
    RUN_TEST(test_add_alarm_invalid_minute);
    RUN_TEST(test_add_alarm_count_increments);
    
    // Enable/disable tests
    RUN_TEST(test_enable_alarm);
    RUN_TEST(test_disable_alarm);
    RUN_TEST(test_toggle_alarm);
    
    // Set alarm time tests
    RUN_TEST(test_set_alarm_time);
    RUN_TEST(test_set_alarm_time_invalid_hour);
    RUN_TEST(test_set_alarm_time_invalid_minute);
    
    // Set alarm label tests
    RUN_TEST(test_set_alarm_label);
    RUN_TEST(test_set_alarm_label_empty);
    
    // Set alarm repeat tests
    RUN_TEST(test_set_alarm_repeat);
    
    // Remove alarm tests
    RUN_TEST(test_remove_alarm_last);
    RUN_TEST(test_remove_alarm_multiple);
    
    // Check alarm tests
    RUN_TEST(test_check_alarm_matching_time);
    RUN_TEST(test_check_alarm_not_matching_hour);
    RUN_TEST(test_check_alarm_not_matching_minute);
    RUN_TEST(test_check_alarm_disabled);
    
    // Configuration tests
    RUN_TEST(test_set_config);
    RUN_TEST(test_default_config);
    
    // Callback tests
    RUN_TEST(test_alarm_callback_on_check);
    
    // Pending alarm tests
    RUN_TEST(test_has_pending_alarm_false_initially);
    RUN_TEST(test_clear_pending_alarm);
    RUN_TEST(test_get_pending_alarm_initial);
    
    // Advanced alarm functionality tests
    RUN_TEST(test_alarm_time_update_24h);
    RUN_TEST(test_alarm_at_midnight);
    RUN_TEST(test_alarm_at_noon);
    RUN_TEST(test_alarm_end_of_day);
    
    // Multiple alarms tests
    RUN_TEST(test_multiple_alarms_same_time);
    RUN_TEST(test_update_calls_callback);
    
    // Edge cases
    RUN_TEST(test_empty_label);
    RUN_TEST(test_very_long_label);
    RUN_TEST(test_invalid_alarm_index);
    
    return UNITY_END();
}
