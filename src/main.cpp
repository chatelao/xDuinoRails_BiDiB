#include <Arduino.h>
#include "BiDiB.h"

BiDiB bidib;

void setup() {
  Serial.begin(115200);
  bidib.begin(Serial);
}

void loop() {
  bidib.update();
}
