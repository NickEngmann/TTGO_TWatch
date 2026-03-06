#include "unity.h"
#include "unity_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test fixture setup */
void setUp(void) {
    /* Run before each test */
}

void tearDown(void) {
    /* Run after each test */
}

/* Test: Verify native build compiles */
void test_native_build_compiles(void) {
    TEST_PASS();
}

/* Test: Verify basic integer operations */
void test_integer_operations(void) {
    int a = 5;
    int b = 3;
    
    TEST_ASSERT_EQUAL_INT(8, a + b);
    TEST_ASSERT_EQUAL_INT(2, a - b);
    TEST_ASSERT_EQUAL_INT(15, a * b);
    TEST_ASSERT_EQUAL_INT(1, a / b);
}

/* Test: Verify basic floating point operations */
void test_floating_point_operations(void) {
    float a = 5.5f;
    float b = 2.5f;
    
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 8.0f, a + b);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, a - b);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 13.75f, a * b);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.2f, a / b);
}

/* Test: Verify basic string operations */
void test_string_operations(void) {
    char str1[] = "Hello";
    char str2[] = " World";
    char result[20];
    
    strcpy(result, str1);
    strcat(result, str2);
    
    TEST_ASSERT_EQUAL_STRING("Hello World", result);
    TEST_ASSERT_EQUAL_UINT8(strlen(result), 11);
}

/* Test: Verify basic array operations */
void test_array_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    
    TEST_ASSERT_EQUAL_INT(1, arr[0]);
    TEST_ASSERT_EQUAL_INT(3, arr[2]);
    TEST_ASSERT_EQUAL_INT(5, arr[4]);
}

/* Test: Verify basic logic operations */
void test_logic_operations(void) {
    int a = 5;
    int b = 3;
    
    TEST_ASSERT_TRUE(a > b);
    TEST_ASSERT_FALSE(a < b);
    TEST_ASSERT_TRUE(a == 5);
    TEST_ASSERT_FALSE(a == 3);
}

/* Test: Verify basic comparison operations */
void test_comparison_operations(void) {
    int a = 10;
    int b = 20;
    int c = 10;
    
    TEST_ASSERT_TRUE(a < b);
    TEST_ASSERT_TRUE(b > a);
    TEST_ASSERT_TRUE(a == c);
    TEST_ASSERT_TRUE(a != b);
}

/* Test: Verify basic bitwise operations */
void test_bitwise_operations(void) {
    int a = 0b1010;
    int b = 0b0101;
    
    TEST_ASSERT_EQUAL_INT(0b1111, a | b);
    TEST_ASSERT_EQUAL_INT(0b0000, a & b);
    TEST_ASSERT_EQUAL_INT(0b1111, a ^ b);
}

/* Test: Verify basic shift operations */
void test_shift_operations(void) {
    int a = 0b0001;
    
    TEST_ASSERT_EQUAL_INT(0b0010, a << 1);
    TEST_ASSERT_EQUAL_INT(0b0100, a << 2);
}

/* Test: Verify basic loop operations */
void test_loop_operations(void) {
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;
    }
    
    TEST_ASSERT_EQUAL_INT(55, sum);
}

/* Test: Verify basic function call operations */
void test_function_call_operations(void) {
    int result = 0;
    result = 5 + 3;
    TEST_ASSERT_EQUAL_INT(8, result);
}

/* Test: Verify basic memory operations */
void test_memory_operations(void) {
    int arr[5];
    for (int i = 0; i < 5; i++) {
        arr[i] = i * 2;
    }
    
    TEST_ASSERT_EQUAL_INT(0, arr[0]);
    TEST_ASSERT_EQUAL_INT(2, arr[1]);
    TEST_ASSERT_EQUAL_INT(4, arr[2]);
    TEST_ASSERT_EQUAL_INT(6, arr[3]);
    TEST_ASSERT_EQUAL_INT(8, arr[4]);
}

/* Test: Verify basic pointer operations */
void test_pointer_operations(void) {
    int a = 5;
    int *ptr = &a;
    
    TEST_ASSERT_EQUAL_INT(5, *ptr);
    TEST_ASSERT_EQUAL_INT(&a, ptr);
}

/* Test: Verify basic structure operations */
void test_structure_operations(void) {
    typedef struct {
        int x;
        int y;
    } Point;
    
    Point p = {10, 20};
    
    TEST_ASSERT_EQUAL_INT(10, p.x);
    TEST_ASSERT_EQUAL_INT(20, p.y);
}

/* Test: Verify basic enum operations */
void test_enum_operations(void) {
    typedef enum {
        RED,
        GREEN,
        BLUE
    } Color;
    
    Color c = RED;
    TEST_ASSERT_EQUAL_INT(0, c);
    
    c = GREEN;
    TEST_ASSERT_EQUAL_INT(1, c);
    
    c = BLUE;
    TEST_ASSERT_EQUAL_INT(2, c);
}

/* Test: Verify basic constant operations */
void test_constant_operations(void) {
    const int PI = 314159;
    TEST_ASSERT_EQUAL_INT(314159, PI);
}

/* Test: Verify basic type conversion operations */
void test_type_conversion_operations(void) {
    int a = 5;
    float b = (float)a;
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 5.0f, b);
}

/* Test: Verify basic conditional operations */
void test_conditional_operations(void) {
    int a = 5;
    int b = 3;
    int result;
    
    result = (a > b) ? a : b;
    TEST_ASSERT_EQUAL_INT(5, result);
}

/* Test: Verify basic arithmetic operations */
void test_arithmetic_operations(void) {
    int a = 10;
    int b = 5;
    
    TEST_ASSERT_EQUAL_INT(15, a + b);
    TEST_ASSERT_EQUAL_INT(5, a - b);
    TEST_ASSERT_EQUAL_INT(50, a * b);
    TEST_ASSERT_EQUAL_INT(2, a / b);
    TEST_ASSERT_EQUAL_INT(0, a % b);
}

/* Test: Verify basic logical operations */
void test_logical_operations(void) {
    int a = 5;
    int b = 3;
    
    TEST_ASSERT_TRUE(a > 0 && b > 0);
    TEST_ASSERT_TRUE(a > 0 || b < 0);
    TEST_ASSERT_FALSE(a < 0 && b < 0);
    TEST_ASSERT_FALSE(a < 0 || b < 0);
}

/* Test: Verify basic string comparison operations */
void test_string_comparison_operations(void) {
    char str1[] = "Hello";
    char str2[] = "Hello";
    char str3[] = "World";
    
    TEST_ASSERT_EQUAL_STRING(str1, str2);
    TEST_ASSERT_NOT_EQUAL_STRING(str1, str3);
}

/* Test: Verify basic string length operations */
void test_string_length_operations(void) {
    char str1[] = "Hello";
    char str2[] = "World";
    
    TEST_ASSERT_EQUAL_UINT8(5, strlen(str1));
    TEST_ASSERT_EQUAL_UINT8(5, strlen(str2));
}

/* Test: Verify basic string copy operations */
void test_string_copy_operations(void) {
    char str1[] = "Hello";
    char str2[10];
    
    strcpy(str2, str1);
    TEST_ASSERT_EQUAL_STRING("Hello", str2);
}

/* Test: Verify basic string concatenate operations */
void test_string_concatenate_operations(void) {
    char str1[20] = "Hello";
    char str2[] = " World";
    
    strcat(str1, str2);
    TEST_ASSERT_EQUAL_STRING("Hello World", str1);
}

/* Test: Verify basic array copy operations */
void test_array_copy_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[5];
    
    memcpy(arr2, arr1, sizeof(arr1));
    
    TEST_ASSERT_EQUAL_INT(1, arr2[0]);
    TEST_ASSERT_EQUAL_INT(2, arr2[1]);
    TEST_ASSERT_EQUAL_INT(3, arr2[2]);
    TEST_ASSERT_EQUAL_INT(4, arr2[3]);
    TEST_ASSERT_EQUAL_INT(5, arr2[4]);
}

/* Test: Verify basic array fill operations */
void test_array_fill_operations(void) {
    int arr[5];
    for (int i = 0; i < 5; i++) {
        arr[i] = i;
    }
    
    TEST_ASSERT_EQUAL_INT(0, arr[0]);
    TEST_ASSERT_EQUAL_INT(1, arr[1]);
    TEST_ASSERT_EQUAL_INT(2, arr[2]);
    TEST_ASSERT_EQUAL_INT(3, arr[3]);
    TEST_ASSERT_EQUAL_INT(4, arr[4]);
}

/* Test: Verify basic array sum operations */
void test_array_sum_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int sum = 0;
    
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    
    TEST_ASSERT_EQUAL_INT(15, sum);
}

/* Test: Verify basic array product operations */
void test_array_product_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int product = 1;
    
    for (int i = 0; i < n; i++) {
        product *= arr[i];
    }
    
    TEST_ASSERT_EQUAL_INT(120, product);
}

/* Test: Verify basic array average operations */
void test_array_average_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int sum = 0;
    float average;
    
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    
    average = (float)sum / (float)n;
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, average);
}

/* Test: Verify basic array min operations */
void test_array_min_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int min = arr[0];
    
    for (int i = 1; i < n; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    
    TEST_ASSERT_EQUAL_INT(1, min);
}

/* Test: Verify basic array max operations */
void test_array_max_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int max = arr[0];
    
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    
    TEST_ASSERT_EQUAL_INT(5, max);
}

/* Test: Verify basic array find operations */
void test_array_find_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target = 3;
    int found = -1;
    
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) {
            found = i;
            break;
        }
    }
    
    TEST_ASSERT_EQUAL_INT(2, found);
}

/* Test: Verify basic array count operations */
void test_array_count_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int count = 0;
    
    for (int i = 0; i < n; i++) {
        count++;
    }
    
    TEST_ASSERT_EQUAL_INT(5, count);
}

/* Test: Verify basic array unique operations */
void test_array_unique_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    int unique_count = 0;
    
    for (int i = 0; i < n; i++) {
        int is_unique = 1;
        for (int j = 0; j < i; j++) {
            if (arr[i] == arr[j]) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT(5, unique_count);
}

/* Test: Verify basic array duplicate operations */
void test_array_duplicate_operations(void) {
    int arr[] = {1, 2, 2, 3, 3, 3};
    int n = sizeof(arr) / sizeof(arr[0]);
    int duplicate_count = 0;
    
    for (int i = 0; i < n; i++) {
        int is_duplicate = 0;
        for (int j = 0; j < i; j++) {
            if (arr[i] == arr[j]) {
                is_duplicate = 1;
                break;
            }
        }
        if (is_duplicate) {
            duplicate_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT(3, duplicate_count);
}

/* Test: Verify basic array intersection operations */
void test_array_intersection_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {3, 4, 5, 6, 7};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int intersection[5];
    int intersection_count = 0;
    
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            if (arr1[i] == arr2[j]) {
                intersection[intersection_count++] = arr1[i];
                break;
            }
        }
    }
    
    TEST_ASSERT_EQUAL_INT(3, intersection_count);
    TEST_ASSERT_EQUAL_INT(3, intersection[0]);
    TEST_ASSERT_EQUAL_INT(4, intersection[1]);
    TEST_ASSERT_EQUAL_INT(5, intersection[2]);
}

/* Test: Verify basic array union operations */
void test_array_union_operations(void) {
    int arr1[] = {1, 2, 3};
    int arr2[] = {3, 4, 5};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int union_arr[10];
    int union_count = 0;
    
    for (int i = 0; i < n1; i++) {
        union_arr[union_count++] = arr1[i];
    }
    
    for (int i = 0; i < n2; i++) {
        int is_in_union = 0;
        for (int j = 0; j < union_count; j++) {
            if (arr2[i] == union_arr[j]) {
                is_in_union = 1;
                break;
            }
        }
        if (!is_in_union) {
            union_arr[union_count++] = arr2[i];
        }
    }
    
    TEST_ASSERT_EQUAL_INT(5, union_count);
    TEST_ASSERT_EQUAL_INT(1, union_arr[0]);
    TEST_ASSERT_EQUAL_INT(2, union_arr[1]);
    TEST_ASSERT_EQUAL_INT(3, union_arr[2]);
    TEST_ASSERT_EQUAL_INT(4, union_arr[3]);
    TEST_ASSERT_EQUAL_INT(5, union_arr[4]);
}

/* Test: Verify basic array difference operations */
void test_array_difference_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {3, 4, 5};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int difference[5];
    int difference_count = 0;
    
    for (int i = 0; i < n1; i++) {
        int is_in_arr2 = 0;
        for (int j = 0; j < n2; j++) {
            if (arr1[i] == arr2[j]) {
                is_in_arr2 = 1;
                break;
            }
        }
        if (!is_in_arr2) {
            difference[difference_count++] = arr1[i];
        }
    }
    
    TEST_ASSERT_EQUAL_INT(2, difference_count);
    TEST_ASSERT_EQUAL_INT(1, difference[0]);
    TEST_ASSERT_EQUAL_INT(2, difference[1]);
}

/* Test: Verify basic array symmetric difference operations */
void test_array_symmetric_difference_operations(void) {
    int arr1[] = {1, 2, 3};
    int arr2[] = {3, 4, 5};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int symmetric_diff[10];
    int symmetric_diff_count = 0;
    
    for (int i = 0; i < n1; i++) {
        int is_in_arr2 = 0;
        for (int j = 0; j < n2; j++) {
            if (arr1[i] == arr2[j]) {
                is_in_arr2 = 1;
                break;
            }
        }
        if (!is_in_arr2) {
            symmetric_diff[symmetric_diff_count++] = arr1[i];
        }
    }
    
    for (int i = 0; i < n2; i++) {
        int is_in_arr1 = 0;
        for (int j = 0; j < n1; j++) {
            if (arr2[i] == arr1[j]) {
                is_in_arr1 = 1;
                break;
            }
        }
        if (!is_in_arr1) {
            symmetric_diff[symmetric_diff_count++] = arr2[i];
        }
    }
    
    TEST_ASSERT_EQUAL_INT(4, symmetric_diff_count);
    TEST_ASSERT_EQUAL_INT(1, symmetric_diff[0]);
    TEST_ASSERT_EQUAL_INT(2, symmetric_diff[1]);
    TEST_ASSERT_EQUAL_INT(4, symmetric_diff[2]);
    TEST_ASSERT_EQUAL_INT(5, symmetric_diff[3]);
}

/* Test: Verify basic array subset operations */
void test_array_subset_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {2, 4};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int is_subset = 1;
    
    for (int i = 0; i < n2; i++) {
        int is_in_arr1 = 0;
        for (int j = 0; j < n1; j++) {
            if (arr2[i] == arr1[j]) {
                is_in_arr1 = 1;
                break;
            }
        }
        if (!is_in_arr1) {
            is_subset = 0;
            break;
        }
    }
    
    TEST_ASSERT_TRUE(is_subset);
}

/* Test: Verify basic array superset operations */
void test_array_superset_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {2, 4};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int is_superset = 1;
    
    for (int i = 0; i < n2; i++) {
        int is_in_arr1 = 0;
        for (int j = 0; j < n1; j++) {
            if (arr2[i] == arr1[j]) {
                is_in_arr1 = 1;
                break;
            }
        }
        if (!is_in_arr1) {
            is_superset = 0;
            break;
        }
    }
    
    TEST_ASSERT_TRUE(is_superset);
}

/* Test: Verify basic array disjoint operations */
void test_array_disjoint_operations(void) {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int is_disjoint = 1;
    
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            if (arr1[i] == arr2[j]) {
                is_disjoint = 0;
                break;
            }
        }
        if (!is_disjoint) {
            break;
        }
    }
    
    TEST_ASSERT_TRUE(is_disjoint);
}

/* Test: Verify basic array equal operations */
void test_array_equal_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {1, 2, 3, 4, 5};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int is_equal = 1;
    
    if (n1 != n2) {
        is_equal = 0;
    } else {
        for (int i = 0; i < n1; i++) {
            if (arr1[i] != arr2[i]) {
                is_equal = 0;
                break;
            }
        }
    }
    
    TEST_ASSERT_TRUE(is_equal);
}

/* Test: Verify basic array not equal operations */
void test_array_not_equal_operations(void) {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {1, 2, 3, 4, 6};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    int is_not_equal = 1;
    
    if (n1 != n2) {
        is_not_equal = 1;
    } else {
        for (int i = 0; i < n1; i++) {
            if (arr1[i] != arr2[i]) {
                is_not_equal = 1;
                break;
            }
        }
    }
    
    TEST_ASSERT_TRUE(is_not_equal);
}

/* Test: Verify basic array empty operations */
void test_array_empty_operations(void) {
    int arr[] = {};
    int n = sizeof(arr) / sizeof(arr[0]);
    TEST_ASSERT_EQUAL_INT(0, n);
}

/* Test: Verify basic array non-empty operations */
void test_array_non_empty_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    TEST_ASSERT_EQUAL_INT(5, n);
}

/* Test: Verify basic array null operations */
void test_array_null_operations(void) {
    int *arr = NULL;
    TEST_ASSERT_NULL(arr);
}

/* Test: Verify basic array non-null operations */
void test_array_non_null_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int *ptr = arr;
    TEST_ASSERT_NOT_NULL(ptr);
}

/* Test: Verify basic array zero operations */
void test_array_zero_operations(void) {
    int arr[] = {0, 0, 0, 0, 0};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(0, arr[i]);
    }
}

/* Test: Verify basic array non-zero operations */
void test_array_non_zero_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_NOT_EQUAL_INT(0, arr[i]);
    }
}

/* Test: Verify basic array positive operations */
void test_array_positive_operations(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_GREATER_THAN_INT(0, arr[i]);
    }
}

/* Test: Verify basic array negative operations */
void test_array_negative_operations(void) {
    int arr[] = {-1, -2, -3, -4, -5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_LESS_THAN_INT(0, arr[i]);
    }
}

/* Test: Verify basic array zero or positive operations */
void test_array_zero_or_positive_operations(void) {
    int arr[] = {0, 1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, arr[i]);
    }
}

/* Test: Verify basic array zero or negative operations */
void test_array_zero_or_negative_operations(void) {
    int arr[] = {0, -1, -2, -3, -4, -5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_LESS_OR_EQUAL_INT(0, arr[i]);
    }
}

/* Test: Verify basic array odd operations */
void test_array_odd_operations(void) {
    int arr[] = {1, 3, 5, 7, 9};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(1, arr[i] % 2);
    }
}

/* Test: Verify basic array even operations */
void test_array_even_operations(void) {
    int arr[] = {2, 4, 6, 8, 10};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(0, arr[i] % 2);
    }
}

/* Test: Verify basic array prime operations */
void test_array_prime_operations(void) {
    int arr[] = {2, 3, 5, 7, 11};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int is_prime = 1;
        for (int j = 2; j * j <= arr[i]; j++) {
            if (arr[i] % j == 0) {
                is_prime = 0;
                break;
            }
        }
        TEST_ASSERT_TRUE(is_prime);
    }
}

/* Test: Verify basic array composite operations */
void test_array_composite_operations(void) {
    int arr[] = {4, 6, 8, 9, 10};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int is_composite = 0;
        for (int j = 2; j * j <= arr[i]; j++) {
            if (arr[i] % j == 0) {
                is_composite = 1;
                break;
            }
        }
        TEST_ASSERT_TRUE(is_composite);
    }
}

/* Test: Verify basic array perfect square operations */
void test_array_perfect_square_operations(void) {
    int arr[] = {1, 4, 9, 16, 25};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int is_perfect_square = 0;
        for (int j = 1; j * j <= arr[i]; j++) {
            if (j * j == arr[i]) {
                is_perfect_square = 1;
                break;
            }
        }
        TEST_ASSERT_TRUE(is_perfect_square);
    }
}

/* Test: Verify basic array perfect cube operations */
void test_array_perfect_cube_operations(void) {
    int arr[] = {1, 8, 27, 64, 125};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int is_perfect_cube = 0;
        for (int j = 1; j * j * j <= arr[i]; j++) {
            if (j * j * j == arr[i]) {
                is_perfect_cube = 1;
                break;
            }
        }
        TEST_ASSERT_TRUE(is_perfect_cube);
    }
}

/* Test: Verify basic array fibonacci operations */
void test_array_fibonacci_operations(void) {
    int arr[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 2; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(arr[i - 1] + arr[i - 2], arr[i]);
    }
}

/* Test: Verify basic array factorial operations */
void test_array_factorial_operations(void) {
    int arr[] = {1, 1, 2, 6, 24, 120};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int factorial = 1;
        for (int j = 1; j <= i; j++) {
            factorial *= j;
        }
        TEST_ASSERT_EQUAL_INT(factorial, arr[i]);
    }
}

/* Test: Verify basic array power operations */
void test_array_power_operations(void) {
    int arr[] = {1, 2, 4, 8, 16, 32};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(1 << i, arr[i]);
    }
}

/* Test: Verify basic array logarithm operations */
void test_array_logarithm_operations(void) {
    int arr[] = {1, 2, 4, 8, 16, 32};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int log2 = 0;
        int temp = arr[i];
        while (temp > 1) {
            temp >>= 1;
            log2++;
        }
        TEST_ASSERT_EQUAL_INT(i, log2);
    }
}

/* Test: Verify basic array square root operations */
void test_array_square_root_operations(void) {
    int arr[] = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int sqrt_val = 0;
        for (int j = 1; j * j <= arr[i]; j++) {
            if (j * j == arr[i]) {
                sqrt_val = j;
                break;
            }
        }
        TEST_ASSERT_EQUAL_INT(i + 1, sqrt_val);
    }
}

/* Test: Verify basic array cube root operations */
void test_array_cube_root_operations(void) {
    int arr[] = {1, 8, 27, 64, 125, 216, 343, 512, 729, 1000};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        int cube_root = 0;
        for (int j = 1; j * j * j <= arr[i]; j++) {
            if (j * j * j == arr[i]) {
                cube_root = j;
                break;
            }
        }
        TEST_ASSERT_EQUAL_INT(i + 1, cube_root);
    }
}

/* Test: Verify basic array golden ratio operations */
void test_array_golden_ratio_operations(void) {
    int arr[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 2; i < n; i++) {
        float golden_ratio = (float)arr[i] / (float)arr[i - 1];
        TEST_ASSERT_FLOAT_WITHIN(0.1f, 1.618f, golden_ratio);
    }
}

/* Test: Verify basic array pi operations */
void test_array_pi_operations(void) {
    float arr[] = {3.14159f, 3.14159f, 3.14159f, 3.14159f, 3.14159f};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14159f, arr[i]);
    }
}

/* Test: Verify basic array e operations */
void test_array_e_operations(void) {
    float arr[] = {2.71828f, 2.71828f, 2.71828f, 2.71828f, 2.71828f};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.71828f, arr[i]);
    }
}
