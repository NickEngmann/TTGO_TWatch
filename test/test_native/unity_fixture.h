#ifndef UNITY_FIXTURE_H
#define UNITY_FIXTURE_H

#include "unity.h"
#include <string.h>

/* Additional macro for testing string inequality */
#define TEST_ASSERT_NOT_EQUAL_STRING(expected, actual) do { \
    if (strcmp((expected), (actual)) == 0) { \
        TEST_FAIL_MESSAGE("Strings are equal but should not be"); \
    } \
} while(0)

#endif /* UNITY_FIXTURE_H */
