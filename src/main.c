#include "TTGO.h"

#ifdef NATIVE_BUILD
#include <stdio.h>

/* Native build entry point */
int main(int argc, char **argv) {
    printf("TTGO T-Watch Native Build\n");
    printf("Platform: Native (for testing)\n");
    printf("\n");
    printf("This is a native build for testing purposes.\n");
    printf("Use 'pio test -e native' to run Unity tests.\n");
    printf("Use 'pio run -e esp32' to build for ESP32 hardware.\n");
    return 0;
}
#else
/* ESP32 build - main entry point */
void setup() {
    TTGOClass *watch = TTGOClass::getWatch();
    watch->begin();
    watch->screen->turnOn();
}

void loop() {
    // Main loop for ESP32
}
#endif
