#include <Arduino.h>
#include "TTGO.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("TTGO T-Watch Library Test");
    
    TTGOClass *twatch = TTGOClass::getWatch();
    if (twatch) {
        twatch->begin(NO_HARDWARE);
        Serial.println("Watch initialized");
    }
}

void loop() {
    delay(1000);
    Serial.println("Running...");
}
