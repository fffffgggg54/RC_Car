#include "config.h"
#include "sensor.h"

Sensor s;

void mainloop(void*){
  while(1){
    Serial.println("value: " + String(s.read()));
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
    1024,
    NULL,
    0,
    nullptr
  );

}

void loop() {}
