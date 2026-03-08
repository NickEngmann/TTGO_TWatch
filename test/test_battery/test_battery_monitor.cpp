#include <unity.h>
#include "../src/BatteryMonitor.cpp"

static BatteryMonitor* bm = nullptr;

static int alertCallCount = 0;
static const char* lastAlertType = nullptr;
static int lastAlertPercentage = -1;

static void resetAlertTracking() {
    alertCallCount = 0;
    lastAlertType = nullptr;
    lastAlertPercentage = -1;
}

static void testAlertCallback(const char* alertType, int percentage) {
    alertCallCount++;
    lastAlertType = alertType;
    lastAlertPercentage = percentage;
}

void setUp(void) {
    bm = new BatteryMonitor();
    resetAlertTracking();
}

void tearDown(void) {
    delete bm;
    bm = nullptr;
}

void test_init(void) {
    bool result = bm->init();
    TEST_ASSERT_TRUE(result);
    BatteryMonitor::Status s = bm->getStatus();
    TEST_ASSERT_TRUE(s.initialized);
}

void test_get_percentage(void) {
    bm->init();
    int pct = bm->getPercentage();
    TEST_ASSERT_EQUAL_INT(85, pct);
}

void test_get_voltage(void) {
    bm->init();
    float v = bm->getVoltage();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 4.02f, v);
}

void test_low_battery_alert(void) {
    bm->init();
    BatteryMonitor::Config cfg = bm->getConfig();
    cfg.enableAlerts = true;
    cfg.lowBatteryThreshold = 20;
    cfg.criticalThreshold = 5;
    bm->setConfig(cfg);
    bm->setAlertCallback(testAlertCallback);

    // Drain from 85 to below 20: 66 updates needed, USB toggles at 200
    for (int i = 0; i < 70; i++) {
        bm->update();
    }
    TEST_ASSERT_GREATER_THAN(0, alertCallCount);
    TEST_ASSERT_TRUE(lastAlertType != nullptr);
}

void test_critical_alert(void) {
    bm->init();
    BatteryMonitor::Config cfg = bm->getConfig();
    cfg.enableAlerts = true;
    cfg.lowBatteryThreshold = 20;
    cfg.criticalThreshold = 5;
    bm->setConfig(cfg);
    bm->setAlertCallback(testAlertCallback);

    // Drain from 85 to below 5: 81 updates needed
    for (int i = 0; i < 85; i++) {
        bm->update();
    }

    TEST_ASSERT_TRUE(lastAlertType != nullptr);
    TEST_ASSERT_EQUAL_STRING("critical", lastAlertType);
    TEST_ASSERT_TRUE(lastAlertPercentage <= 5);
}

void test_charging_detection(void) {
    bm->init();
    TEST_ASSERT_FALSE(bm->isCharging());

    // USB toggles on at update 200
    for (int i = 0; i < 200; i++) {
        bm->update();
    }
    TEST_ASSERT_TRUE(bm->isCharging());
}

void test_power_saving_activation(void) {
    bm->init();
    BatteryMonitor::Config cfg = bm->getConfig();
    cfg.enablePowerSaving = true;
    cfg.lowBatteryThreshold = 20;
    bm->setConfig(cfg);

    TEST_ASSERT_FALSE(bm->getStatus().powerSavingActive);

    // Drain below 20 and stay not charging
    for (int i = 0; i < 70; i++) {
        bm->update();
    }

    BatteryMonitor::Status s = bm->getStatus();
    TEST_ASSERT_TRUE(s.isLowBattery);
    TEST_ASSERT_FALSE(s.isCharging);
    TEST_ASSERT_TRUE(s.powerSavingActive);
}

void test_config_update(void) {
    bm->init();
    BatteryMonitor::Config cfg;
    cfg.lowBatteryThreshold = 30;
    cfg.criticalThreshold = 10;
    cfg.enableAlerts = false;
    cfg.enablePowerSaving = false;
    cfg.pollIntervalMs = 10000;
    bm->setConfig(cfg);

    BatteryMonitor::Config got = bm->getConfig();
    TEST_ASSERT_EQUAL_INT(30, got.lowBatteryThreshold);
    TEST_ASSERT_EQUAL_INT(10, got.criticalThreshold);
    TEST_ASSERT_FALSE(got.enableAlerts);
    TEST_ASSERT_FALSE(got.enablePowerSaving);
    TEST_ASSERT_EQUAL_UINT32(10000, got.pollIntervalMs);
}

void test_reset_alerts(void) {
    bm->init();
    BatteryMonitor::Config cfg = bm->getConfig();
    cfg.enableAlerts = false;
    bm->setConfig(cfg);
    bm->setAlertCallback(testAlertCallback);

    for (int i = 0; i < 90; i++) {
        bm->update();
    }

    TEST_ASSERT_EQUAL_INT(0, alertCallCount);
}

void test_callback_fires(void) {
    bm->init();
    BatteryMonitor::Config cfg = bm->getConfig();
    cfg.enableAlerts = true;
    cfg.lowBatteryThreshold = 80;
    cfg.criticalThreshold = 1;
    bm->setConfig(cfg);
    bm->setAlertCallback(testAlertCallback);

    // Drain from 85 past 80: about 6 updates
    for (int i = 0; i < 6; i++) {
        bm->update();
    }

    TEST_ASSERT_GREATER_THAN(0, alertCallCount);
    TEST_ASSERT_TRUE(lastAlertType != nullptr);
    TEST_ASSERT_EQUAL_STRING("low_battery", lastAlertType);
}

void test_multiple_updates_drain_battery(void) {
    bm->init();
    int initial = bm->getPercentage();

    for (int i = 0; i < 10; i++) {
        bm->update();
    }

    int after = bm->getPercentage();
    TEST_ASSERT_TRUE(after < initial);
    TEST_ASSERT_EQUAL_INT(initial - 10, after);
}

void test_usb_connection_detection(void) {
    bm->init();
    TEST_ASSERT_FALSE(bm->getStatus().isUSBConnected);

    // USB toggles at 200 updates
    for (int i = 0; i < 200; i++) {
        bm->update();
    }
    TEST_ASSERT_TRUE(bm->getStatus().isUSBConnected);
}

void test_temperature_reading(void) {
    bm->init();
    float temp = bm->getStatus().temperature;
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 25.0f, temp);

    for (int i = 0; i < 20; i++) {
        bm->update();
    }
    float tempAfter = bm->getStatus().temperature;
    TEST_ASSERT_TRUE(tempAfter >= temp);
}

void test_deinit(void) {
    bm->init();
    TEST_ASSERT_TRUE(bm->getStatus().initialized);
    bm->deinit();
    TEST_ASSERT_FALSE(bm->getStatus().initialized);
}

void test_update_without_init(void) {
    bm->update();
    TEST_ASSERT_EQUAL_INT(0, bm->getPercentage());
}

void test_enable_power_saving_toggle(void) {
    bm->init();
    bm->enablePowerSaving(false);
    BatteryMonitor::Config cfg = bm->getConfig();
    TEST_ASSERT_FALSE(cfg.enablePowerSaving);
    TEST_ASSERT_FALSE(bm->getStatus().powerSavingActive);

    bm->enablePowerSaving(true);
    cfg = bm->getConfig();
    TEST_ASSERT_TRUE(cfg.enablePowerSaving);
}

void test_set_lvgl_widget(void) {
    bm->init();
    int dummyWidget = 42;
    bm->setLVGLWidget(&dummyWidget);
    bm->update();
    TEST_ASSERT_TRUE(bm->getStatus().initialized);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_init);
    RUN_TEST(test_get_percentage);
    RUN_TEST(test_get_voltage);
    RUN_TEST(test_low_battery_alert);
    RUN_TEST(test_critical_alert);
    RUN_TEST(test_charging_detection);
    RUN_TEST(test_power_saving_activation);
    RUN_TEST(test_config_update);
    RUN_TEST(test_reset_alerts);
    RUN_TEST(test_callback_fires);
    RUN_TEST(test_multiple_updates_drain_battery);
    RUN_TEST(test_usb_connection_detection);
    RUN_TEST(test_temperature_reading);
    RUN_TEST(test_deinit);
    RUN_TEST(test_update_without_init);
    RUN_TEST(test_enable_power_saving_toggle);
    RUN_TEST(test_set_lvgl_widget);

    return UNITY_END();
}
