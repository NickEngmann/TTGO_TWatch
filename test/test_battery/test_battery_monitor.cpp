#include <unity.h>
#include "../../src/BatteryMonitor.cpp"

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

void test_battery_init(void) {
    TEST_ASSERT_TRUE(bm->init());
    TEST_ASSERT_EQUAL(100, bm->getPercentage());
}

void test_battery_voltage(void) {
    bm->init();
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 4.2f, bm->getVoltage());
}

void test_battery_drain(void) {
    bm->init();
    for (int i = 0; i < 10; i++) bm->update();
    TEST_ASSERT_TRUE(bm->getPercentage() < 100);
}

void test_low_battery_alert(void) {
    bm->init();
    bm->setAlertCallback(testAlertCallback);
    // Drain to below 20%
    for (int i = 0; i < 85; i++) bm->update();
    TEST_ASSERT_TRUE(bm->isLowBattery());
    TEST_ASSERT_GREATER_THAN(0, alertCallCount);
}

void test_critical_alert(void) {
    bm->init();
    bm->setAlertCallback(testAlertCallback);
    // Drain to below 5%
    for (int i = 0; i < 98; i++) bm->update();
    TEST_ASSERT_TRUE(bm->isCritical());
}

void test_charging_detection(void) {
    bm->init();
    // Initially not charging
    TEST_ASSERT_FALSE(bm->isCharging());
}

void test_power_saving_activates_on_low(void) {
    bm->init();
    for (int i = 0; i < 85; i++) bm->update();
    TEST_ASSERT_TRUE(bm->isPowerSavingActive());
}

void test_config_update(void) {
    bm->init();
    BatteryMonitor::Config cfg;
    cfg.lowBatteryThreshold = 30;
    cfg.criticalThreshold = 10;
    bm->setConfig(cfg);
    TEST_ASSERT_EQUAL(30, bm->getConfig().lowBatteryThreshold);
}

void test_alert_disabled(void) {
    bm->init();
    BatteryMonitor::Config cfg;
    cfg.enableAlerts = false;
    bm->setConfig(cfg);
    bm->setAlertCallback(testAlertCallback);
    for (int i = 0; i < 98; i++) bm->update();
    TEST_ASSERT_EQUAL(0, alertCallCount);
}

void test_edge_triggered_alerts(void) {
    bm->init();
    bm->setAlertCallback(testAlertCallback);
    // Cross low threshold
    for (int i = 0; i < 82; i++) bm->update();
    int firstCount = alertCallCount;
    // Continue draining - should not re-fire low alert
    bm->update();
    TEST_ASSERT_EQUAL(firstCount, alertCallCount);
}

void test_usb_detection(void) {
    bm->init();
    TEST_ASSERT_FALSE(bm->isUSBConnected());
}

void test_temperature(void) {
    bm->init();
    bm->update();
    TEST_ASSERT_FLOAT_WITHIN(5.0f, 25.0f, bm->getTemperature());
}

void test_deinit(void) {
    bm->init();
    bm->deinit();
    bm->update();  // Should not crash
    // After deinit, update is no-op so percentage stays at init value
    TEST_ASSERT_EQUAL(100, bm->getPercentage());
}

void test_power_saving_toggle(void) {
    bm->init();
    bm->enablePowerSaving(false);
    for (int i = 0; i < 85; i++) bm->update();
    TEST_ASSERT_FALSE(bm->isPowerSavingActive());
}

void test_status_struct(void) {
    bm->init();
    bm->update();
    BatteryMonitor::Status s = bm->getStatus();
    TEST_ASSERT_TRUE(s.percentage > 0);
    TEST_ASSERT_TRUE(s.voltage > 3.0f);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_battery_init);
    RUN_TEST(test_battery_voltage);
    RUN_TEST(test_battery_drain);
    RUN_TEST(test_low_battery_alert);
    RUN_TEST(test_critical_alert);
    RUN_TEST(test_charging_detection);
    RUN_TEST(test_power_saving_activates_on_low);
    RUN_TEST(test_config_update);
    RUN_TEST(test_alert_disabled);
    RUN_TEST(test_edge_triggered_alerts);
    RUN_TEST(test_usb_detection);
    RUN_TEST(test_temperature);
    RUN_TEST(test_deinit);
    RUN_TEST(test_power_saving_toggle);
    RUN_TEST(test_status_struct);
    return UNITY_END();
}
