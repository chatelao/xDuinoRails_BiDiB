#include <Arduino.h>
#include "BiDiB.h"

#if !defined(UNIT_TEST)

#if defined(ARDUINO) // This will be true for the 'uno' environment
BiDiB bidib;

void setup() {
  Serial.begin(115200);
  bidib.begin(Serial);
}

void loop() {
  bidib.update();
}

#else // This will be true for the 'native' environment

// For a native build (`pio run -e native`), we need a main function to link successfully.
// The library itself isn't intended to run as a standalone native application,
// so a simple empty main is sufficient.
int main(int argc, char **argv) {
    return 0;
}

#endif // defined(ARDUINO)

#endif // !defined(UNIT_TEST)
