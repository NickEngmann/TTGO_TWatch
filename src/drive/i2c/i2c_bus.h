#ifndef TTGO_I2CBUF_H
#define TTGO_I2CBUF_H

#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef NATIVE_BUILD

/* Native build stub for I2CBus */
typedef struct {
    void *_port;
    void *_i2c_mux;
} I2CBus;

static inline I2CBus *i2c_bus_new(TwoWire *port, int sda, int scl) {
    (void)port;
    (void)sda;
    (void)scl;
    return (I2CBus *)0x12345678;
}

static inline void i2c_bus_delete(I2CBus *bus) {
    (void)bus;
}

static inline void i2c_bus_scan(I2CBus *bus) {
    (void)bus;
}

static inline uint16_t i2c_bus_readBytes(I2CBus *bus, uint8_t addr, uint8_t *data, uint16_t len, uint16_t delay_ms) {
    (void)bus;
    (void)addr;
    (void)data;
    (void)len;
    (void)delay_ms;
    return 0;
}

static inline uint16_t i2c_bus_readBytes_reg(I2CBus *bus, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len) {
    (void)bus;
    (void)addr;
    (void)reg;
    (void)data;
    (void)len;
    return 0;
}

static inline uint16_t i2c_bus_writeBytes(I2CBus *bus, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len) {
    (void)bus;
    (void)addr;
    (void)reg;
    (void)data;
    (void)len;
    return 0;
}

static inline bool i2c_bus_deviceProbe(I2CBus *bus, uint8_t addr) {
    (void)bus;
    (void)addr;
    return false;
}

static inline void i2c_bus_setClock(I2CBus *bus, uint32_t c) {
    (void)bus;
    (void)c;
}

static inline int i2c_bus_readBytes_u16(I2CBus *bus, int addr, uint16_t reg, uint8_t *data, int len) {
    (void)bus;
    (void)addr;
    (void)reg;
    (void)data;
    (void)len;
    return 0;
}

static inline int i2c_bus_writeBytes_u16(I2CBus *bus, int addr, uint16_t reg, uint8_t *data, int len) {
    (void)bus;
    (void)addr;
    (void)reg;
    (void)data;
    (void)len;
    return 0;
}

static inline TwoWire *i2c_bus_getHandler(I2CBus *bus) {
    (void)bus;
    return &Wire;
}

#else

class I2CBus
{
public:
    I2CBus(TwoWire &port = Wire, int sda = 21, int scl = 22)
    {
        _port = &port;
        _port->begin(sda, scl);
        _i2c_mux = xSemaphoreCreateRecursiveMutex();
        // _port->setClock(400000);
    };
    void scan();
    uint16_t readBytes(uint8_t addr, uint8_t *data, uint16_t len, uint16_t delay_ms = 0);
    uint16_t readBytes(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
    uint16_t writeBytes(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
    bool deviceProbe(uint8_t addr);
    void setClock(uint32_t c)
    {
        _port->setClock(c);
    }

    int readBytes_u16(int addr, uint16_t reg, uint8_t *data, int len);
    int writeBytes_u16(int addr, uint16_t reg, uint8_t *data, int len);
    TwoWire *getHandler()
    {
        return _port;
    }
private:
    TwoWire *_port;
    SemaphoreHandle_t _i2c_mux = NULL;
};

#endif /* NATIVE_BUILD */

#endif /* TTGO_I2CBUF_H */