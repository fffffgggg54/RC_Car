#include "config.h"
#include "gps.h"
#include "sensor.h"

void setup() {

  Serial.begin(115200);
  while (!Serial) {}

  Sensor s;
  s.begin();

}

void loop() {}
