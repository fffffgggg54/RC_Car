#include "config.h"
#include "sensor.h"
//#include "servo_log.h"

Sensor s;

void mainloop(void*){
  while(1){
    Serial.printf("value: %d\n\r", s.read());
    vTaskDelay(200);
  }
}

void setup() {

  Serial.begin(115200);
  while (!Serial) {}

  pinMode(23, INPUT_PULLUP);
  s.begin(23);

  xTaskCreate(
    mainloop,
    "MAIN",
    2048,
    NULL,
    0,
    nullptr
  );

}

void loop() {}
