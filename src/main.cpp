#include <Arduino.h>
#include "BiDiB.h"

#ifndef UNIT_TEST

BiDiB bidib;

void setup() {
  Serial.begin(115200);
  bidib.begin(Serial);
}

void loop() {
  bidib.update();
}

#endif // UNIT_TEST
