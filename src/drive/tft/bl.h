#ifndef __BL_H
#define __BL_H

#include <Arduino.h>
#include <Ticker.h>

#ifdef NATIVE_BUILD

/* Native build stubs for PWMBase and derived classes */

typedef struct {
    uint8_t _pin;
    uint8_t _channel;
} PWMBase;

typedef struct {
    PWMBase base;
    bool _on;
    uint8_t _level;
} BackLight;

typedef struct {
    PWMBase base;
    double _freq;
    void *_tick;
} PWMToneBase;

typedef struct {
    PWMToneBase base;
} Motor;

typedef struct {
    PWMToneBase base;
} Buzzer;

static inline PWMBase* PWMBase_new(uint8_t pin, uint8_t channel) {
    (void)pin;
    (void)channel;
    return (PWMBase*)0x12345678;
}

static inline void PWMBase_delete(PWMBase* pwm) {
    (void)pwm;
}

static inline void PWMBase_begin(PWMBase* pwm) {
    (void)pwm;
}

static inline void PWMBase_adjust(PWMBase* pwm, uint8_t level) {
    (void)pwm;
    (void)level;
}

static inline BackLight* BackLight_new(uint8_t pin, uint8_t channel) {
    (void)pin;
    (void)channel;
    return (BackLight*)0x12345678;
}

static inline void BackLight_delete(BackLight* bl) {
    (void)bl;
}

static inline uint8_t BackLight_getLevel(BackLight* bl) {
    (void)bl;
    return 0;
}

static inline void BackLight_adjust(BackLight* bl, uint8_t level) {
    (void)bl;
    (void)level;
}

static inline bool BackLight_isOn(BackLight* bl) {
    (void)bl;
    return false;
}

static inline void BackLight_on(BackLight* bl) {
    (void)bl;
}

static inline void BackLight_off(BackLight* bl) {
    (void)bl;
}

static inline bool BackLight_reverse(BackLight* bl) {
    (void)bl;
    return false;
}

static inline PWMToneBase* PWMToneBase_new(uint8_t pin, uint8_t channel, int freq) {
    (void)pin;
    (void)channel;
    (void)freq;
    return (PWMToneBase*)0x12345678;
}

static inline void PWMToneBase_delete(PWMToneBase* pwm) {
    (void)pwm;
}

static inline void PWMToneBase_begin(PWMToneBase* pwm) {
    (void)pwm;
}

static inline void PWMToneBase_onec(PWMToneBase* pwm, int duration) {
    (void)pwm;
    (void)duration;
}

static inline Motor* Motor_new(uint8_t pin, uint8_t channel, int freq) {
    (void)pin;
    (void)channel;
    (void)freq;
    return (Motor*)0x12345678;
}

static inline void Motor_delete(Motor* motor) {
    (void)motor;
}

static inline Buzzer* Buzzer_new(uint8_t pin, uint8_t channel, int freq) {
    (void)pin;
    (void)channel;
    (void)freq;
    return (Buzzer*)0x12345678;
}

static inline void Buzzer_delete(Buzzer* buzzer) {
    (void)buzzer;
}

#else

class PWMBase
{
public:
    PWMBase(uint8_t pin, uint8_t channel)
    {
        _pin = pin;
        _channel = channel;
    };

    virtual ~PWMBase()
    {
        ledcDetachPin(_pin);
    };

    virtual void begin()
    {
        ledcSetup(_channel, 1000, 8);
        ledcAttachPin(_pin, _channel);
        ledcWrite(_channel, 0);
    };

    virtual void adjust(uint8_t level)
    {
        ledcWrite(_channel, level);
    };

protected:
    uint8_t _pin;
    uint8_t _channel;
};

class BackLight : public PWMBase
{
public:
    BackLight(uint8_t pin, uint8_t channel = 0) : PWMBase(pin, channel)
    {
    };
    uint8_t getLevel()
    {
        return _level;
    }
    void adjust(uint8_t level)
    {
        _level = level;
        PWMBase::adjust(level);
        _on = true;
    };

    bool isOn()
    {
        return _on;
    };

    void on()
    {
        _on = true;
        ledcWrite(_channel, _level);
    };

    void off()
    {
        _on = false;
        ledcWrite(_channel, 0);
    };

    bool reverse()
    {
        _on ? off() : on();
        return _on;
    };

private:
    bool _on = false;
    uint8_t _level = 255;
};


class PWMToneBase : public PWMBase
{
public:
    PWMToneBase(uint8_t pin, uint8_t channel, int freq) : PWMBase(pin, channel)
    {
        _freq = freq;
        _tick = nullptr;
    };

    virtual ~PWMToneBase()
    {
        if (_tick != nullptr) {
            delete _tick;
        }
    }

    virtual void begin()
    {
        PWMBase::begin();
        _tick = new Ticker;
    };

    virtual void onec(int duration = 200)
    {
        ledcWriteTone(_channel, _freq);
        _tick->once_ms<uint8_t>(duration, [](uint8_t channel) {
            ledcWriteTone(channel, 0);
        }, _channel);
    };

protected:
    double _freq;
    Ticker *_tick;

};

class Motor : public PWMToneBase
{
public:
    Motor(uint8_t pin, uint8_t channel = 4, int freq = 1000) : PWMToneBase(pin, channel, freq)
    {
    };
};


class Buzzer : public PWMToneBase
{
public:
    Buzzer(uint8_t pin, uint8_t channel = 2, int freq = 1000) : PWMToneBase(pin, channel, freq)
    {
    };
};

#endif /* NATIVE_BUILD */

#endif
