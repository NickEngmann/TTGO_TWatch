#ifndef Arduino_h
#define Arduino_h

#ifdef NATIVE_BUILD

/* Arduino.h stub for native build - minimal set with renamed functions to avoid conflicts */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* Basic Arduino types */
typedef uint8_t byte;
typedef uint16_t word;
typedef int16_t sint16_t;
typedef uint16_t uint16_t;
typedef int32_t sint32_t;
typedef uint32_t uint32_t;
typedef int64_t sint64_t;
typedef uint64_t uint64_t;

/* Arduino constants */
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define INPUT_PULLDOWN 3
#define CHANGE 1
#define FALLING 2
#define RISING 3

/* Arduino functions - only the ones needed by TTGO library, renamed to avoid conflicts */
static inline void pinMode_stub(uint8_t pin, uint8_t mode) {
    (void)pin;
    (void)mode;
}

static inline void digitalWrite_stub(uint8_t pin, uint8_t val) {
    (void)pin;
    (void)val;
}

static inline int digitalRead_stub(uint8_t pin) {
    (void)pin;
    return LOW;
}

static inline void analogWrite_stub(uint8_t pin, int val) {
    (void)pin;
    (void)val;
}

static inline int analogRead_stub(uint8_t pin) {
    (void)pin;
    return 0;
}

static inline void delay_stub(unsigned long ms) {
    (void)ms;
}

static inline void delayMicroseconds_stub(unsigned int us) {
    (void)us;
}

static inline unsigned long millis_stub(void) {
    return 0;
}

static inline unsigned long micros_stub(void) {
    return 0;
}

static inline void noInterrupts_stub(void) {
    /* Stub */
}

static inline void interrupts_stub(void) {
    /* Stub */
}

static inline void attachInterrupt_stub(uint8_t pin, void (*handler)(void), int mode) {
    (void)pin;
    (void)handler;
    (void)mode;
}

static inline void detachInterrupt_stub(uint8_t pin) {
    (void)pin;
}

static inline void randomSeed_stub(unsigned int seed) {
    (void)seed;
}

static inline int random_stub(int max) {
    (void)max;
    return 0;
}

static inline int random_range_stub(int min, int max) {
    (void)min;
    (void)max;
    return 0;
}

static inline int map_stub(int x, int in_min, int in_max, int out_min, int out_max) {
    (void)x;
    (void)in_min;
    (void)in_max;
    (void)out_min;
    (void)out_max;
    return 0;
}

static inline void noTone_stub(uint8_t pin) {
    (void)pin;
}

static inline void tone_stub(uint8_t pin, int frequency, unsigned long duration) {
    (void)pin;
    (void)frequency;
    (void)duration;
}

static inline void shiftOut_stub(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value) {
    (void)dataPin;
    (void)clockPin;
    (void)bitOrder;
    (void)value;
}

static inline uint8_t shiftIn_stub(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    (void)dataPin;
    (void)clockPin;
    (void)bitOrder;
    return 0;
}

static inline void setup_stub() {
    /* Stub */
}

static inline void loop_stub() {
    /* Stub */
}

static inline void yield_stub(void) {
    /* Stub */
}

static inline void cli_stub(void) {
    noInterrupts_stub();
}

static inline void sei_stub(void) {
    interrupts_stub();
}

static inline void bitSet_stub(volatile uint8_t *addr, uint8_t bit) {
    (void)addr;
    (void)bit;
}

static inline void bitClear_stub(volatile uint8_t *addr, uint8_t bit) {
    (void)addr;
    (void)bit;
}

static inline void bitWrite_stub(volatile uint8_t *addr, uint8_t bit, uint8_t value) {
    (void)addr;
    (void)bit;
    (void)value;
}

static inline uint8_t bitRead_stub(uint8_t val, uint8_t bit) {
    (void)val;
    (void)bit;
    return 0;
}

static inline void pulseIn_stub(uint8_t pin, uint8_t state, unsigned long timeout) {
    (void)pin;
    (void)state;
    (void)timeout;
}

static inline unsigned long pulseInLong_stub(uint8_t pin, uint8_t state, unsigned long timeout) {
    (void)pin;
    (void)state;
    (void)timeout;
    return 0;
}

/* String functions - only the ones needed, renamed to avoid conflicts */
static inline int my_atoi(const char *str) {
    (void)str;
    return 0;
}

static inline long my_atol(const char *str) {
    (void)str;
    return 0;
}

static inline int my_abs(int x) {
    return x < 0 ? -x : x;
}

static inline long my_labs(long x) {
    return x < 0 ? -x : x;
}

static inline double my_atof(const char *str) {
    (void)str;
    return 0.0;
}

/* Array functions - only the ones needed, renamed to avoid conflicts */
static inline int my_strcmp(const char *s1, const char *s2) {
    (void)s1;
    (void)s2;
    return 0;
}

static inline int my_strncmp(const char *s1, const char *s2, size_t n) {
    (void)s1;
    (void)s2;
    (void)n;
    return 0;
}

static inline size_t my_strlen(const char *s) {
    (void)s;
    return 0;
}

static inline int my_min(int a, int b) {
    return a < b ? a : b;
}

static inline int my_max(int a, int b) {
    return a > b ? a : b;
}

static inline long my_min_l(long a, long b) {
    return a < b ? a : b;
}

static inline long my_max_l(long a, long b) {
    return a > b ? a : b;
}

static inline unsigned int my_min_u(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}

static inline unsigned int my_max_u(unsigned int a, unsigned int b) {
    return a > b ? a : b;
}

static inline float my_abs_f(float x) {
    return x < 0 ? -x : x;
}

static inline float my_min_f(float a, float b) {
    return a < b ? a : b;
}

static inline float my_max_f(float a, float b) {
    return a > b ? a : b;
}

/* Serial functions - only the ones needed, renamed to avoid conflicts */
static inline void Serial_begin_stub(unsigned long baud) {
    (void)baud;
}

static inline void Serial_end_stub(void) {
    /* Stub */
}

static inline int Serial_available_stub(void) {
    return 0;
}

static inline int Serial_peek_stub(void) {
    return -1;
}

static inline int Serial_read_stub(void) {
    return -1;
}

static inline size_t Serial_write_byte(uint8_t c) {
    (void)c;
    return 0;
}

static inline size_t Serial_write_buf(const uint8_t *buf, size_t size) {
    (void)buf;
    (void)size;
    return 0;
}

static inline void Serial_flush_stub(void) {
    /* Stub */
}

static inline int Serial_print_str(const char *str) {
    (void)str;
    return 0;
}

static inline int Serial_println_str(const char *str) {
    (void)str;
    return 0;
}

static inline int Serial_print_int(int n) {
    (void)n;
    return 0;
}

static inline int Serial_println_int(int n) {
    (void)n;
    return 0;
}

static inline int Serial_print_float(float n, int decimals) {
    (void)n;
    (void)decimals;
    return 0;
}

static inline int Serial_println_float(float n, int decimals) {
    (void)n;
    (void)decimals;
    return 0;
}

#endif /* NATIVE_BUILD */

#endif /* Arduino_h */
