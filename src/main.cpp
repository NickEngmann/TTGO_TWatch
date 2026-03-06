/*
 * Main entry point for TTGO T-Watch
 * This file provides the Arduino setup() and loop() functions
 */

#include "TTGO.h"

TTGOClass *ttgo = TTGOClass::getWatch();

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    Serial.println("TTGO T-Watch Setup");
    
    // Initialize the watch using begin() method
    ttgo->begin();
    
    // Turn on display backlight
    #ifdef LILYGO_WATCH_HAS_AXP202
    ttgo->power->setLDO3(true);
    ttgo->power->setLDO2(true);
    #endif
    
    Serial.println("Watch initialized");
}

void loop() {
    // Main loop - watch runs in background
    // Display updates and sensor readings are handled by the watch library
    delay(100);
}
