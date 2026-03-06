#include "TTGO.h"

void setup() {
    TTGOClass *watch = TTGOClass::getWatch();
    if (watch) {
        watch->begin();
    }
}

void loop() {
    delay(1000);
}
