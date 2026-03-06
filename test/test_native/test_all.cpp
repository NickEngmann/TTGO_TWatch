#include <unity.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ============================================
// AXP20X Power Management Tests
// ============================================

// Voltage calculation logic from AXP20X
float calculateVoltage(uint16_t raw_value, float scale_factor) {
    return raw_value * scale_factor;
}

float calculateBatteryVoltage(uint16_t h8, uint8_t l4) {
    // AXP202 battery voltage: (h8 << 4) | l4, scale = 1.4mV
    uint16_t raw = (h8 << 4) | (l4 & 0x0F);
    return raw * 0.0014f;
}

float calculateBatteryCurrent(uint16_t h8, uint8_t l5) {
    // AXP202 battery current: (h8 << 5) | l5, scale = 0.5mA
    uint16_t raw = (h8 << 5) | (l5 & 0x1F);
    return raw * 0.0005f;
}

// IRQ status extraction
bool isChargingIRQ(uint8_t int_status) {
    return (int_status & 0x08) != 0;
}

bool isChargingDoneIRQ(uint8_t int_status) {
    return (int_status & 0x10) != 0;
}

bool isBattPlugInIRQ(uint8_t int_status) {
    return (int_status & 0x20) != 0;
}

bool isBattRemoveIRQ(uint8_t int_status) {
    return (int_status & 0x40) != 0;
}

// Battery percentage calculation (simplified)
int calculateBatteryPercentage(uint8_t raw_percentage) {
    if (raw_percentage > 100) return 100;
    if (raw_percentage < 0) return 0;
    return (int)raw_percentage;
}

// ============================================
// Button State Machine Tests
// ============================================

typedef enum {
    BUTTON_STATE_IDLE = 0,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HELD,
    BUTTON_STATE_RELEASED
} ButtonState_t;

typedef struct {
    ButtonState_t state;
    uint32_t press_time;
    uint32_t hold_threshold;
    uint32_t debounce_delay;
    uint32_t last_input_time;
    bool is_pressed;
    bool is_released;
    bool is_held;
    bool last_input;
} ButtonState;

void button_init(ButtonState *btn, uint32_t hold_threshold, uint32_t debounce_delay) {
    btn->state = BUTTON_STATE_IDLE;
    btn->press_time = 0;
    btn->hold_threshold = hold_threshold;
    btn->debounce_delay = debounce_delay;
    btn->last_input_time = 0;
    btn->is_pressed = false;
    btn->is_released = false;
    btn->is_held = false;
    btn->last_input = false;
}

void button_update(ButtonState *btn, bool input_pin, uint32_t *tick) {
    if (tick == NULL) {
        return;
    }
    (*tick)++;
    
    if (input_pin != btn->last_input) {
        btn->last_input_time = *tick;
        btn->last_input = input_pin;
    }
    
    if (input_pin) {
        if (*tick - btn->last_input_time >= btn->debounce_delay) {
            if (btn->state == BUTTON_STATE_IDLE) {
                btn->state = BUTTON_STATE_PRESSED;
                btn->is_pressed = true;
                btn->press_time = 0;
            } else if (btn->state == BUTTON_STATE_PRESSED) {
                btn->press_time++;
                if (btn->press_time >= btn->hold_threshold) {
                    btn->state = BUTTON_STATE_HELD;
                    btn->is_held = true;
                    btn->is_pressed = false;
                }
            }
        }
    } else {
        if (btn->state == BUTTON_STATE_PRESSED || btn->state == BUTTON_STATE_HELD) {
            btn->state = BUTTON_STATE_RELEASED;
            btn->is_released = true;
            btn->is_pressed = false;
            btn->is_held = false;
        } else if (btn->state == BUTTON_STATE_IDLE) {
            btn->is_released = true;
        }
    }
}

void button_reset(ButtonState *btn) {
    btn->is_pressed = false;
    btn->is_released = false;
    btn->is_held = false;
    if (btn->state == BUTTON_STATE_RELEASED) {
        btn->state = BUTTON_STATE_IDLE;
    }
}

bool button_was_pressed(ButtonState *btn) {
    return btn->is_pressed;
}

bool button_was_released(ButtonState *btn) {
    return btn->is_released;
}

bool button_is_held(ButtonState *btn) {
    return btn->is_held;
}

ButtonState_t get_button_state(ButtonState *btn) {
    return btn->state;
}

// ============================================
// I2C Bus Tests
// ============================================

typedef struct {
    uint8_t device_address;
    uint8_t register_address;
    uint8_t data[256];
    size_t data_length;
    bool initialized;
} I2CDevice;

bool i2c_device_init(I2CDevice *dev, uint8_t address) {
    if (dev == NULL) {
        return false;
    }
    dev->device_address = address;
    dev->register_address = 0;
    dev->data_length = 0;
    memset(dev->data, 0, sizeof(dev->data));
    dev->initialized = true;
    return true;
}

bool i2c_write_register(I2CDevice *dev, uint8_t reg, uint8_t value) {
    if (!dev->initialized) {
        return false;
    }
    dev->register_address = reg;
    dev->data[reg] = value;
    return true;
}

uint8_t i2c_read_register(I2CDevice *dev, uint8_t reg) {
    if (!dev->initialized) {
        return 0;
    }
    return dev->data[reg];
}

bool i2c_write_multiple(I2CDevice *dev, uint8_t reg, const uint8_t *data, size_t len) {
    if (!dev->initialized || data == NULL || len == 0 || len > 256) {
        return false;
    }
    dev->register_address = reg;
    for (size_t i = 0; i < len && i < 256; i++) {
        dev->data[reg + i] = data[i];
    }
    dev->data_length = len;
    return true;
}

bool i2c_read_multiple(I2CDevice *dev, uint8_t reg, uint8_t *data, size_t len) {
    if (!dev->initialized || data == NULL || len == 0 || len > 256) {
        return false;
    }
    for (size_t i = 0; i < len && i < 256; i++) {
        data[i] = dev->data[reg + i];
    }
    return true;
}

bool i2c_device_is_initialized(I2CDevice *dev) {
    return dev != NULL && dev->initialized;
}

uint8_t i2c_get_device_address(I2CDevice *dev) {
    if (!dev || !dev->initialized) {
        return 0;
    }
    return dev->device_address;
}

// ============================================
// Touch Screen Tests
// ============================================

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t touch_id;
    bool touched;
    bool released;
} TouchPoint;

typedef struct {
    TouchPoint points[5];
    uint8_t touch_count;
    uint16_t width;
    uint16_t height;
    bool initialized;
} TouchScreen;

bool touch_screen_init(TouchScreen *ts, uint16_t w, uint16_t h) {
    if (ts == NULL) {
        return false;
    }
    ts->width = w;
    ts->height = h;
    ts->touch_count = 0;
    ts->initialized = true;
    memset(ts->points, 0, sizeof(ts->points));
    return true;
}

void touch_screen_clear(TouchScreen *ts) {
    if (ts == NULL || !ts->initialized) {
        return;
    }
    ts->touch_count = 0;
    memset(ts->points, 0, sizeof(ts->points));
}

bool touch_screen_add_point(TouchScreen *ts, uint16_t x, uint16_t y, uint8_t id) {
    if (ts == NULL || !ts->initialized) {
        return false;
    }
    if (x >= ts->width || y >= ts->height) {
        return false;
    }
    if (ts->touch_count >= 5) {
        return false;
    }
    ts->points[ts->touch_count].x = x;
    ts->points[ts->touch_count].y = y;
    ts->points[ts->touch_count].touch_id = id;
    ts->points[ts->touch_count].touched = true;
    ts->touch_count++;
    return true;
}

bool touch_screen_remove_point(TouchScreen *ts, uint8_t id) {
    if (ts == NULL || !ts->initialized) {
        return false;
    }
    for (uint8_t i = 0; i < ts->touch_count; i++) {
        if (ts->points[i].touch_id == id) {
            ts->points[i].released = true;
            for (uint8_t j = i; j < ts->touch_count - 1; j++) {
                ts->points[j] = ts->points[j + 1];
            }
            ts->touch_count--;
            return true;
        }
    }
    return false;
}

uint8_t touch_screen_get_count(TouchScreen *ts) {
    if (ts == NULL || !ts->initialized) {
        return 0;
    }
    return ts->touch_count;
}

bool touch_screen_get_point(TouchScreen *ts, uint8_t index, TouchPoint *point) {
    if (ts == NULL || !ts->initialized || point == NULL) {
        return false;
    }
    if (index >= ts->touch_count) {
        return false;
    }
    *point = ts->points[index];
    return true;
}

bool touch_screen_is_touched(TouchScreen *ts) {
    if (ts == NULL || !ts->initialized) {
        return false;
    }
    return ts->touch_count > 0;
}

bool touch_screen_was_released(TouchScreen *ts, uint8_t index) {
    if (ts == NULL || !ts->initialized) {
        return false;
    }
    if (index >= ts->touch_count) {
        return false;
    }
    return ts->points[index].released;
}

bool touch_screen_get_center(TouchScreen *ts, uint16_t *cx, uint16_t *cy) {
    if (ts == NULL || !ts->initialized || cx == NULL || cy == NULL) {
        return false;
    }
    if (ts->touch_count == 0) {
        return false;
    }
    
    uint32_t sum_x = 0;
    uint32_t sum_y = 0;
    
    for (uint8_t i = 0; i < ts->touch_count; i++) {
        sum_x += ts->points[i].x;
        sum_y += ts->points[i].y;
    }
    
    *cx = (uint16_t)(sum_x / ts->touch_count);
    *cy = (uint16_t)(sum_y / ts->touch_count);
    return true;
}

bool touch_screen_is_within_bounds(TouchScreen *ts, uint16_t x, uint16_t y) {
    if (ts == NULL || !ts->initialized) {
        return false;
    }
    return (x < ts->width && y < ts->height);
}

// ============================================
// Test Setup/Teardown
// ============================================

void setUp(void) {
}

void tearDown(void) {
}

// ============================================
// AXP20X Test Cases
// ============================================

void test_calculateBatteryVoltage_full_scale(void) {
    uint16_t raw = 0xFFFF;
    uint32_t voltage_mv = raw * 14;  // 1.4mV * 1000 = 14
    TEST_ASSERT_EQUAL_UINT32(917490, voltage_mv);  // 0xFFFF = 65535, 65535 * 14 = 917490
}

void test_calculateBatteryVoltage_zero(void) {
    uint16_t raw = 0x0000;
    uint32_t voltage_mv = raw * 14;
    TEST_ASSERT_EQUAL_UINT32(0, voltage_mv);
}

void test_calculateBatteryCurrent_full_scale(void) {
    uint16_t raw = 0x1F;
    uint32_t current_ma = raw * 5;  // 0.5mA * 10 = 5
    TEST_ASSERT_EQUAL_UINT32(155, current_ma);  // 0x1F * 5 = 155 (0.155mA)
}

void test_calculateBatteryCurrent_zero(void) {
    uint16_t raw = 0x00;
    uint32_t current_ma = raw * 5;
    TEST_ASSERT_EQUAL_UINT32(0, current_ma);
}

void test_isChargingIRQ_true(void) {
    uint8_t status = 0x08;
    TEST_ASSERT_TRUE(isChargingIRQ(status));
}

void test_isChargingIRQ_false(void) {
    uint8_t status = 0x00;
    TEST_ASSERT_FALSE(isChargingIRQ(status));
}

void test_isChargingDoneIRQ_true(void) {
    uint8_t status = 0x10;
    TEST_ASSERT_TRUE(isChargingDoneIRQ(status));
}

void test_isChargingDoneIRQ_false(void) {
    uint8_t status = 0x00;
    TEST_ASSERT_FALSE(isChargingDoneIRQ(status));
}

void test_isBattPlugInIRQ_true(void) {
    uint8_t status = 0x20;
    TEST_ASSERT_TRUE(isBattPlugInIRQ(status));
}

void test_isBattPlugInIRQ_false(void) {
    uint8_t status = 0x00;
    TEST_ASSERT_FALSE(isBattPlugInIRQ(status));
}

void test_isBattRemoveIRQ_true(void) {
    uint8_t status = 0x40;
    TEST_ASSERT_TRUE(isBattRemoveIRQ(status));
}

void test_isBattRemoveIRQ_false(void) {
    uint8_t status = 0x00;
    TEST_ASSERT_FALSE(isBattRemoveIRQ(status));
}

void test_calculateBatteryPercentage_valid(void) {
    int result = calculateBatteryPercentage(75);
    TEST_ASSERT_EQUAL_INT(75, result);
}

void test_calculateBatteryPercentage_zero(void) {
    int result = calculateBatteryPercentage(0);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_calculateBatteryPercentage_max(void) {
    int result = calculateBatteryPercentage(100);
    TEST_ASSERT_EQUAL_INT(100, result);
}

void test_calculateBatteryPercentage_clamped_low(void) {
    int result = calculateBatteryPercentage(0);
    TEST_ASSERT_EQUAL_INT(0, result);  // Should return 0
}

void test_calculateBatteryPercentage_clamped_high(void) {
    int result = calculateBatteryPercentage(150);
    TEST_ASSERT_EQUAL_INT(100, result);
}

void test_getRegistH8L5_basic(void) {
    uint8_t h8 = 0x12;
    uint8_t l5 = 0x34;
    uint16_t result = (h8 << 5) | (l5 & 0x1F);
    TEST_ASSERT_EQUAL_UINT16(596, result);  // 0x12 << 5 = 0x180 = 384, 0x34 & 0x1F = 0x14 = 20, 384 + 20 = 404
}

void test_getRegistH8L5_zero(void) {
    uint8_t h8 = 0x00;
    uint8_t l5 = 0x00;
    uint16_t result = (h8 << 5) | (l5 & 0x1F);
    TEST_ASSERT_EQUAL_UINT16(0x000, result);
}

void test_getRegistResult_basic(void) {
    uint8_t h8 = 0x12;
    uint8_t l4 = 0x34;
    uint16_t result = (h8 << 4) | (l4 & 0x0F);
    TEST_ASSERT_EQUAL_UINT16(0x124, result);  // 0x12 << 4 = 0x120, 0x34 & 0x0F = 0x04, 0x120 | 0x04 = 0x124
}

// ============================================
// Button Test Cases
// ============================================

void test_button_init_default(void) {
    ButtonState btn;
    button_init(&btn, 1000, 50);
    TEST_ASSERT_EQUAL(BUTTON_STATE_IDLE, btn.state);
    TEST_ASSERT_EQUAL_UINT32(0, btn.press_time);
    TEST_ASSERT_EQUAL_UINT32(1000, btn.hold_threshold);
    TEST_ASSERT_EQUAL_UINT32(50, btn.debounce_delay);
    TEST_ASSERT_FALSE(btn.is_pressed);
    TEST_ASSERT_FALSE(btn.is_released);
    TEST_ASSERT_FALSE(btn.is_held);
}

void test_button_press_detection(void) {
    ButtonState btn;
    button_init(&btn, 1000, 50);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, false, &tick);
    }
    button_update(&btn, true, &tick);
    
    TEST_ASSERT_TRUE(button_was_pressed(&btn));
    TEST_ASSERT_EQUAL(BUTTON_STATE_PRESSED, btn.state);
}

void test_button_hold_detection(void) {
    ButtonState btn;
    button_init(&btn, 100, 50);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, false, &tick);
    }
    button_update(&btn, true, &tick);
    
    for (uint32_t i = 0; i < 100; i++) {
        button_update(&btn, true, &tick);
    }
    
    TEST_ASSERT_TRUE(button_is_held(&btn));
    TEST_ASSERT_EQUAL(BUTTON_STATE_HELD, btn.state);
}

void test_button_release_detection(void) {
    ButtonState btn;
    button_init(&btn, 1000, 50);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, false, &tick);
    }
    button_update(&btn, true, &tick);
    button_update(&btn, false, &tick);
    
    TEST_ASSERT_TRUE(button_was_released(&btn));
    TEST_ASSERT_EQUAL(BUTTON_STATE_RELEASED, btn.state);
}

void test_button_reset(void) {
    ButtonState btn;
    button_init(&btn, 1000, 50);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, false, &tick);
    }
    button_update(&btn, true, &tick);
    button_update(&btn, false, &tick);
    
    TEST_ASSERT_TRUE(button_was_released(&btn));
    
    button_reset(&btn);
    
    TEST_ASSERT_FALSE(button_was_released(&btn));
    TEST_ASSERT_EQUAL(BUTTON_STATE_IDLE, btn.state);
}

void test_button_debounce_timing(void) {
    ButtonState btn;
    button_init(&btn, 1000, 100);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, true, &tick);
    }
    
    TEST_ASSERT_FALSE(button_was_pressed(&btn));
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, true, &tick);
    }
    
    TEST_ASSERT_TRUE(button_was_pressed(&btn));
}

void test_button_hold_threshold(void) {
    ButtonState btn;
    button_init(&btn, 50, 10);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 10; i++) {
        button_update(&btn, false, &tick);
    }
    button_update(&btn, true, &tick);
    
    TEST_ASSERT_FALSE(button_is_held(&btn));
    
    for (uint32_t i = 0; i < 40; i++) {
        button_update(&btn, true, &tick);
    }
    
    TEST_ASSERT_TRUE(button_is_held(&btn));
}

void test_button_state_transitions(void) {
    ButtonState btn;
    button_init(&btn, 1000, 50);
    uint32_t tick = 0;
    
    for (uint32_t i = 0; i < 50; i++) {
        button_update(&btn, false, &tick);
    }
    button_update(&btn, true, &tick);
    TEST_ASSERT_EQUAL(BUTTON_STATE_PRESSED, btn.state);
    
    button_update(&btn, false, &tick);
    TEST_ASSERT_EQUAL(BUTTON_STATE_RELEASED, btn.state);
    
    button_reset(&btn);
    TEST_ASSERT_EQUAL(BUTTON_STATE_IDLE, btn.state);
}

// ============================================
// I2C Bus Test Cases
// ============================================

void test_i2c_device_init_valid(void) {
    I2CDevice dev;
    bool result = i2c_device_init(&dev, 0x34);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(i2c_device_is_initialized(&dev));
    TEST_ASSERT_EQUAL_UINT8(0x34, i2c_get_device_address(&dev));
}

void test_i2c_device_init_null(void) {
    bool result = i2c_device_init(NULL, 0x34);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_device_init_zero_address(void) {
    I2CDevice dev;
    bool result = i2c_device_init(&dev, 0x00);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0x00, i2c_get_device_address(&dev));
}

void test_i2c_write_register_valid(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    bool result = i2c_write_register(&dev, 0x01, 0xFF);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0xFF, i2c_read_register(&dev, 0x01));
}

void test_i2c_write_register_uninitialized(void) {
    I2CDevice dev;
    bool result = i2c_write_register(&dev, 0x01, 0xFF);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_write_register_zero_value(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    bool result = i2c_write_register(&dev, 0x00, 0x00);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0x00, i2c_read_register(&dev, 0x00));
}

void test_i2c_read_register_valid(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    i2c_write_register(&dev, 0x02, 0xAB);
    uint8_t value = i2c_read_register(&dev, 0x02);
    TEST_ASSERT_EQUAL_UINT8(0xAB, value);
}

void test_i2c_read_register_uninitialized(void) {
    I2CDevice dev;
    uint8_t value = i2c_read_register(&dev, 0x01);
    TEST_ASSERT_EQUAL_UINT8(0x00, value);
}

void test_i2c_read_register_default(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t value = i2c_read_register(&dev, 0x01);
    TEST_ASSERT_EQUAL_UINT8(0x00, value);
}

void test_i2c_write_multiple_valid(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    bool result = i2c_write_multiple(&dev, 0x10, data, 5);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0x01, i2c_read_register(&dev, 0x10));
    TEST_ASSERT_EQUAL_UINT8(0x05, i2c_read_register(&dev, 0x14));
}

void test_i2c_write_multiple_null_data(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    bool result = i2c_write_multiple(&dev, 0x10, NULL, 5);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_write_multiple_zero_length(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t data[] = {0x01};
    bool result = i2c_write_multiple(&dev, 0x10, data, 0);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_write_multiple_too_long(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t data[300];
    bool result = i2c_write_multiple(&dev, 0x10, data, 300);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_read_multiple_valid(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55};
    i2c_write_multiple(&dev, 0x10, data, 5);
    
    uint8_t read_data[5];
    bool result = i2c_read_multiple(&dev, 0x10, read_data, 5);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0x11, read_data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x55, read_data[4]);
}

void test_i2c_read_multiple_null_data(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    bool result = i2c_read_multiple(&dev, 0x10, NULL, 5);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_read_multiple_zero_length(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t data[5];
    bool result = i2c_read_multiple(&dev, 0x10, data, 0);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_read_multiple_too_long(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    uint8_t data[300];
    bool result = i2c_read_multiple(&dev, 0x10, data, 300);
    TEST_ASSERT_FALSE(result);
}

void test_i2c_device_is_initialized_valid(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    TEST_ASSERT_TRUE(i2c_device_is_initialized(&dev));
}

void test_i2c_device_is_initialized_uninitialized(void) {
    I2CDevice dev;
    TEST_ASSERT_FALSE(i2c_device_is_initialized(&dev));
}

void test_i2c_device_is_initialized_null(void) {
    TEST_ASSERT_FALSE(i2c_device_is_initialized(NULL));
}

void test_i2c_write_register_max_value(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    bool result = i2c_write_register(&dev, 0xFF, 0xFF);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0xFF, i2c_read_register(&dev, 0xFF));
}

void test_i2c_read_register_max_value(void) {
    I2CDevice dev;
    i2c_device_init(&dev, 0x34);
    i2c_write_register(&dev, 0xFF, 0x55);
    uint8_t value = i2c_read_register(&dev, 0xFF);
    TEST_ASSERT_EQUAL_UINT8(0x55, value);
}

// ============================================
// Touch Screen Test Cases
// ============================================

void test_touch_screen_init_valid(void) {
    TouchScreen ts;
    bool result = touch_screen_init(&ts, 320, 240);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(ts.initialized);
    TEST_ASSERT_EQUAL_UINT16(320, ts.width);
    TEST_ASSERT_EQUAL_UINT16(240, ts.height);
    TEST_ASSERT_EQUAL_UINT8(0, ts.touch_count);
}

void test_touch_screen_init_null(void) {
    bool result = touch_screen_init(NULL, 320, 240);
    TEST_ASSERT_FALSE(result);
}

void test_touch_screen_init_zero_size(void) {
    TouchScreen ts;
    bool result = touch_screen_init(&ts, 0, 0);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0, ts.width);
    TEST_ASSERT_EQUAL_UINT16(0, ts.height);
}

void test_touch_screen_add_point_valid(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    bool result = touch_screen_add_point(&ts, 100, 150, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(1, ts.touch_count);
    
    TouchPoint point;
    touch_screen_get_point(&ts, 0, &point);
    TEST_ASSERT_EQUAL_UINT16(100, point.x);
    TEST_ASSERT_EQUAL_UINT16(150, point.y);
    TEST_ASSERT_EQUAL_UINT8(1, point.touch_id);
}

void test_touch_screen_add_point_out_of_bounds(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    bool result = touch_screen_add_point(&ts, 400, 300, 1);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT8(0, ts.touch_count);
}

void test_touch_screen_add_point_null_screen(void) {
    bool result = touch_screen_add_point(NULL, 100, 150, 1);
    TEST_ASSERT_FALSE(result);
}

void test_touch_screen_max_touches(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    
    for (int i = 0; i < 5; i++) {
        touch_screen_add_point(&ts, i * 50, i * 40, i);
    }
    TEST_ASSERT_EQUAL_UINT8(5, ts.touch_count);
    
    bool result = touch_screen_add_point(&ts, 300, 200, 99);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT8(5, ts.touch_count);
}

void test_touch_screen_remove_point_valid(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    touch_screen_add_point(&ts, 200, 100, 2);
    
    bool result = touch_screen_remove_point(&ts, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(1, ts.touch_count);
    
    TouchPoint point;
    touch_screen_get_point(&ts, 0, &point);
    TEST_ASSERT_EQUAL_UINT8(2, point.touch_id);
}

void test_touch_screen_remove_point_invalid_id(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    
    bool result = touch_screen_remove_point(&ts, 99);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT8(1, ts.touch_count);
}

void test_touch_screen_clear_valid(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    touch_screen_add_point(&ts, 200, 100, 2);
    
    touch_screen_clear(&ts);
    TEST_ASSERT_EQUAL_UINT8(0, ts.touch_count);
}

void test_touch_screen_clear_null(void) {
    touch_screen_clear(NULL);
}

void test_touch_screen_is_touched_true(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    TEST_ASSERT_TRUE(touch_screen_is_touched(&ts));
}

void test_touch_screen_is_touched_false(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    TEST_ASSERT_FALSE(touch_screen_is_touched(&ts));
}

void test_touch_screen_is_touched_null(void) {
    TEST_ASSERT_FALSE(touch_screen_is_touched(NULL));
}

void test_touch_screen_get_center_single(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    
    uint16_t cx, cy;
    bool result = touch_screen_get_center(&ts, &cx, &cy);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(100, cx);
    TEST_ASSERT_EQUAL_UINT16(150, cy);
}

void test_touch_screen_get_center_multiple(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 0, 0, 1);
    touch_screen_add_point(&ts, 200, 200, 2);
    
    uint16_t cx, cy;
    bool result = touch_screen_get_center(&ts, &cx, &cy);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(100, cx);
    TEST_ASSERT_EQUAL_UINT16(100, cy);
}

void test_touch_screen_get_center_null(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    uint16_t cx, cy;
    bool result = touch_screen_get_center(&ts, NULL, &cy);
    TEST_ASSERT_FALSE(result);
}

void test_touch_screen_is_within_bounds_true(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    TEST_ASSERT_TRUE(touch_screen_is_within_bounds(&ts, 100, 150));
}

void test_touch_screen_is_within_bounds_false(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    TEST_ASSERT_FALSE(touch_screen_is_within_bounds(&ts, 320, 240));
    TEST_ASSERT_FALSE(touch_screen_is_within_bounds(&ts, 400, 300));
}

void test_touch_screen_is_within_bounds_null(void) {
    TEST_ASSERT_FALSE(touch_screen_is_within_bounds(NULL, 100, 150));
}

void test_touch_screen_get_point_valid(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    
    TouchPoint point;
    bool result = touch_screen_get_point(&ts, 0, &point);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(100, point.x);
    TEST_ASSERT_EQUAL_UINT16(150, point.y);
}

void test_touch_screen_get_point_invalid_index(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    touch_screen_add_point(&ts, 100, 150, 1);
    
    TouchPoint point;
    bool result = touch_screen_get_point(&ts, 5, &point);
    TEST_ASSERT_FALSE(result);
}

void test_touch_screen_get_point_null(void) {
    TouchScreen ts;
    touch_screen_init(&ts, 320, 240);
    bool result = touch_screen_get_point(&ts, 0, NULL);
    TEST_ASSERT_FALSE(result);
}

// ============================================
// Main Test Runner
// ============================================

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // AXP20X Tests
    RUN_TEST(test_calculateBatteryVoltage_full_scale);
    RUN_TEST(test_calculateBatteryVoltage_zero);
    RUN_TEST(test_calculateBatteryCurrent_full_scale);
    RUN_TEST(test_calculateBatteryCurrent_zero);
    RUN_TEST(test_isChargingIRQ_true);
    RUN_TEST(test_isChargingIRQ_false);
    RUN_TEST(test_isChargingDoneIRQ_true);
    RUN_TEST(test_isChargingDoneIRQ_false);
    RUN_TEST(test_isBattPlugInIRQ_true);
    RUN_TEST(test_isBattPlugInIRQ_false);
    RUN_TEST(test_isBattRemoveIRQ_true);
    RUN_TEST(test_isBattRemoveIRQ_false);
    RUN_TEST(test_calculateBatteryPercentage_valid);
    RUN_TEST(test_calculateBatteryPercentage_zero);
    RUN_TEST(test_calculateBatteryPercentage_max);
    RUN_TEST(test_calculateBatteryPercentage_clamped_low);
    RUN_TEST(test_calculateBatteryPercentage_clamped_high);
    RUN_TEST(test_getRegistH8L5_basic);
    RUN_TEST(test_getRegistH8L5_zero);
    RUN_TEST(test_getRegistResult_basic);
    
    // Button Tests
    RUN_TEST(test_button_init_default);
    RUN_TEST(test_button_press_detection);
    RUN_TEST(test_button_hold_detection);
    RUN_TEST(test_button_release_detection);
    RUN_TEST(test_button_reset);
    RUN_TEST(test_button_debounce_timing);
    RUN_TEST(test_button_hold_threshold);
    RUN_TEST(test_button_state_transitions);
    
    // I2C Bus Tests
    RUN_TEST(test_i2c_device_init_valid);
    RUN_TEST(test_i2c_device_init_null);
    RUN_TEST(test_i2c_device_init_zero_address);
    RUN_TEST(test_i2c_write_register_valid);
    RUN_TEST(test_i2c_write_register_uninitialized);
    RUN_TEST(test_i2c_write_register_zero_value);
    RUN_TEST(test_i2c_read_register_valid);
    RUN_TEST(test_i2c_read_register_uninitialized);
    RUN_TEST(test_i2c_read_register_default);
    RUN_TEST(test_i2c_write_multiple_valid);
    RUN_TEST(test_i2c_write_multiple_null_data);
    RUN_TEST(test_i2c_write_multiple_zero_length);
    RUN_TEST(test_i2c_write_multiple_too_long);
    RUN_TEST(test_i2c_read_multiple_valid);
    RUN_TEST(test_i2c_read_multiple_null_data);
    RUN_TEST(test_i2c_read_multiple_zero_length);
    RUN_TEST(test_i2c_read_multiple_too_long);
    RUN_TEST(test_i2c_device_is_initialized_valid);
    RUN_TEST(test_i2c_device_is_initialized_uninitialized);
    RUN_TEST(test_i2c_device_is_initialized_null);
    RUN_TEST(test_i2c_write_register_max_value);
    RUN_TEST(test_i2c_read_register_max_value);
    
    // Touch Screen Tests
    RUN_TEST(test_touch_screen_init_valid);
    RUN_TEST(test_touch_screen_init_null);
    RUN_TEST(test_touch_screen_init_zero_size);
    RUN_TEST(test_touch_screen_add_point_valid);
    RUN_TEST(test_touch_screen_add_point_out_of_bounds);
    RUN_TEST(test_touch_screen_add_point_null_screen);
    RUN_TEST(test_touch_screen_max_touches);
    RUN_TEST(test_touch_screen_remove_point_valid);
    RUN_TEST(test_touch_screen_remove_point_invalid_id);
    RUN_TEST(test_touch_screen_clear_valid);
    RUN_TEST(test_touch_screen_clear_null);
    RUN_TEST(test_touch_screen_is_touched_true);
    RUN_TEST(test_touch_screen_is_touched_false);
    RUN_TEST(test_touch_screen_is_touched_null);
    RUN_TEST(test_touch_screen_get_center_single);
    RUN_TEST(test_touch_screen_get_center_multiple);
    RUN_TEST(test_touch_screen_get_center_null);
    RUN_TEST(test_touch_screen_is_within_bounds_true);
    RUN_TEST(test_touch_screen_is_within_bounds_false);
    RUN_TEST(test_touch_screen_is_within_bounds_null);
    RUN_TEST(test_touch_screen_get_point_valid);
    RUN_TEST(test_touch_screen_get_point_invalid_index);
    RUN_TEST(test_touch_screen_get_point_null);
    
    return UNITY_END();
}
