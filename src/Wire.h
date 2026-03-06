#ifndef Wire_h
#define Wire_h

#ifdef NATIVE_BUILD

#include <stdint.h>
#include <stddef.h>

/* Wire.h stub for native build - minimal set with renamed functions to avoid conflicts */

typedef struct {
    int initialized;
} TwoWire;

static inline void Wire_begin_stub(TwoWire *wire) {
    if (wire) wire->initialized = 1;
}

static inline void Wire_end_stub(TwoWire *wire) {
    if (wire) wire->initialized = 0;
}

static inline void Wire_beginTransmission_stub(TwoWire *wire, uint8_t device) {
    (void)wire;
    (void)device;
}

static inline uint8_t Wire_endTransmission_stub(TwoWire *wire) {
    (void)wire;
    return 0;
}

static inline void Wire_requestFrom_stub(TwoWire *wire, uint8_t device, uint8_t quantity) {
    (void)wire;
    (void)device;
    (void)quantity;
}

static inline uint8_t Wire_read_stub(TwoWire *wire) {
    (void)wire;
    return 0;
}

static inline void Wire_write_stub(TwoWire *wire, uint8_t data) {
    (void)wire;
    (void)data;
}

static inline void Wire_onReceive_stub(TwoWire *wire, void (*function)(int)) {
    (void)wire;
    (void)function;
}

static inline void Wire_onRequest_stub(TwoWire *wire, void (*function)(void)) {
    (void)wire;
    (void)function;
}

static inline int Wire_begin_stub(int sda, int scl) {
    (void)sda;
    (void)scl;
    return 0;
}

static inline int Wire_end_stub() {
    return 0;
}

static TwoWire _Wire_instance = {0};
static TwoWire _Wire1_instance = {0};

#define Wire _Wire_instance
#define Wire1 _Wire1_instance

#endif /* NATIVE_BUILD */

#endif /* Wire_h */
