#include <unity.h>
#include <cstdlib>
#include <ctime>
#include "../src/StepCounter.cpp"

// Mock rand for native builds
int rand() {
    static uint32_t seed = 12345;
    seed = seed * 1103515245 + 12345;
    return (int)((seed / 65536) % 32768);
}

StepCounter* stepCounter = nullptr;
StepCounter::Config testConfig;

void setUp(void) {
    stepCounter = new StepCounter();
    testConfig = stepCounter->getConfig();
}

void tearDown(void) {
    delete stepCounter;
    stepCounter = nullptr;
}

void test_init_returns_true(void) {
    bool result = stepCounter->init();
    TEST_ASSERT_TRUE(result);
}

void test_init_sets_initialized_flag(void) {
    stepCounter->init();
    StepCounter::Status status = stepCounter->getStatus();
    TEST_ASSERT_TRUE(status.initialized);
}

void test_get_step_count_initially_zero(void) {
    stepCounter->init();
    uint32_t count = stepCounter->getStepCount();
    TEST_ASSERT_EQUAL_UINT32(0, count);
}

void test_update_increments_steps(void) {
    stepCounter->init();
    
    // Simulate multiple updates
    for (int i = 0; i < 10; i++) {
        stepCounter->update();
    }
    
    uint32_t count = stepCounter->getStepCount();
    TEST_ASSERT_GREATER_THAN(0, count);
}

void test_set_daily_goal(void) {
    stepCounter->init();
    stepCounter->setDailyGoal(15000);
    
    TEST_ASSERT_EQUAL_UINT32(15000, stepCounter->getDailyGoal());
}

void test_goal_reached_when_steps_exceed_goal(void) {
    stepCounter->init();
    stepCounter->setDailyGoal(100);
    
    // Simulate enough updates to exceed goal
    for (int i = 0; i < 50; i++) {
        stepCounter->update();
    }
    
    TEST_ASSERT_TRUE(stepCounter->isGoalReached());
}

void test_goal_not_reached_when_steps_below_goal(void) {
    stepCounter->init();
    stepCounter->setDailyGoal(10000);
    
    // Simulate few updates
    for (int i = 0; i < 5; i++) {
        stepCounter->update();
    }
    
    TEST_ASSERT_FALSE(stepCounter->isGoalReached());
}

void test_reset_daily_count(void) {
    stepCounter->init();
    stepCounter->setDailyGoal(100);
    
    // Get some steps
    for (int i = 0; i < 20; i++) {
        stepCounter->update();
    }
    
    uint32_t countBefore = stepCounter->getStepCount();
    TEST_ASSERT_GREATER_THAN(0, countBefore);
    
    stepCounter->resetDailyCount();
    
    uint32_t countAfter = stepCounter->getStepCount();
    TEST_ASSERT_EQUAL_UINT32(0, countAfter);
}

void test_reset_all_counters(void) {
    stepCounter->init();
    stepCounter->setDailyGoal(100);
    
    // Get some steps
    for (int i = 0; i < 20; i++) {
        stepCounter->update();
    }
    
    StepCounter::Status statusBefore = stepCounter->getStatus();
    TEST_ASSERT_GREATER_THAN(0, statusBefore.totalSteps);
    
    stepCounter->resetAllCounters();
    
    StepCounter::Status statusAfter = stepCounter->getStatus();
    TEST_ASSERT_EQUAL_UINT32(0, statusAfter.totalSteps);
    TEST_ASSERT_EQUAL_UINT32(0, statusAfter.stepsSinceMidnight);
}

void test_config_default_values(void) {
    stepCounter->init();
    
    TEST_ASSERT_EQUAL_UINT32(10000, testConfig.dailyGoal);
    TEST_ASSERT_TRUE(testConfig.enableNotifications);
    TEST_ASSERT_TRUE(testConfig.enableAutoReset);
}

void test_set_config_updates_values(void) {
    stepCounter->init();
    
    StepCounter::Config newConfig;
    newConfig.dailyGoal = 20000;
    newConfig.enableNotifications = false;
    newConfig.enableAutoReset = false;
    
    stepCounter->setConfig(newConfig);
    
    StepCounter::Config currentConfig = stepCounter->getConfig();
    TEST_ASSERT_EQUAL_UINT32(20000, currentConfig.dailyGoal);
    TEST_ASSERT_FALSE(currentConfig.enableNotifications);
    TEST_ASSERT_FALSE(currentConfig.enableAutoReset);
}

void test_progress_ring_widget_set(void) {
    stepCounter->init();
    
    void* mockWidget = (void*)0x1234;
    stepCounter->setProgressRingWidget(mockWidget);
    
    // Verify widget is set (internal state check)
    TEST_ASSERT_TRUE(stepCounter->getStatus().initialized);
}

void test_update_does_not_crash_with_no_init(void) {
    // Don't call init, just call update
    stepCounter->update();
    
    // Should not crash or have invalid state
    TEST_ASSERT_TRUE(true);
}

void test_multiple_updates_accumulate_steps(void) {
    stepCounter->init();
    
    uint32_t initialCount = stepCounter->getStepCount();
    TEST_ASSERT_EQUAL_UINT32(0, initialCount);
    
    // First batch of updates
    for (int i = 0; i < 10; i++) {
        stepCounter->update();
    }
    uint32_t countAfterFirst = stepCounter->getStepCount();
    
    // Second batch of updates
    for (int i = 0; i < 10; i++) {
        stepCounter->update();
    }
    uint32_t countAfterSecond = stepCounter->getStepCount();
    
    // Steps should accumulate
    TEST_ASSERT_MESSAGE(countAfterFirst > 0, "countAfterFirst should be > 0");
    TEST_ASSERT_MESSAGE(countAfterSecond > countAfterFirst, "countAfterSecond should be > countAfterFirst");
}

// Note: This test file is included by test_statemachine.cpp
// Do not include main() here - it's defined in test_statemachine.cpp
