/*
 * Test runner for Unity framework
 * This file is used by PlatformIO to run native tests
 */

#ifdef NATIVE_BUILD

#include <stdio.h>
#include <stdlib.h>
#include "unity.h"

/* Forward declarations from test files */
extern void test_watch_wakeup_logic(void);
extern void test_watch_sleep_logic(void);
extern void test_battery_level_calculation(void);
extern void test_charging_status_check(void);
extern void test_time_remaining_calculation(void);
extern void test_watch_state_transitions(void);
extern void test_alarm_enable_disable(void);
extern void test_battery_edge_cases(void);

void setUp(void) {
    /* Common setup for all tests */
}

void tearDown(void) {
    /* Common cleanup for all tests */
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    printf("\n=== TTGO T-Watch Native Tests ===\n\n");
    
    /* Watch Logic Tests */
    printf("Running Watch Logic tests...\n");
    RUN_TEST(test_watch_wakeup_logic);
    RUN_TEST(test_watch_sleep_logic);
    RUN_TEST(test_battery_level_calculation);
    RUN_TEST(test_charging_status_check);
    RUN_TEST(test_time_remaining_calculation);
    RUN_TEST(test_watch_state_transitions);
    RUN_TEST(test_alarm_enable_disable);
    RUN_TEST(test_battery_edge_cases);
    
    printf("\n=== All Tests Complete ===\n\n");
    
    UNITY_END();
    return 0;
}

#endif /* NATIVE_BUILD */
