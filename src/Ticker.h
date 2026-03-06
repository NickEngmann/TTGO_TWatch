#ifndef Ticker_h
#define Ticker_h

#ifdef NATIVE_BUILD

/* Ticker.h stub for native build */

typedef struct {
    int active;
} Ticker;

static inline Ticker* new_Ticker() {
    static Ticker ticker;
    ticker.active = 1;
    return &ticker;
}

static inline void delete_Ticker(Ticker* ticker) {
    (void)ticker;
}

static inline void Ticker_attach_ms(Ticker* ticker, unsigned long interval, void (*callback)()) {
    (void)ticker;
    (void)interval;
    (void)callback;
}

static inline void Ticker_detach(Ticker* ticker) {
    (void)ticker;
}

static inline void Ticker_detachAll() {
    /* Stub */
}

#endif /* NATIVE_BUILD */

#endif /* Ticker_h */
