#include "config.h"
#include "sensor.h"
#include "servo_log.h"

Sensor pot;
ServoLog s;

void mainloop(void*){
  while(1){

    float p = pot.read()/268374015.0;

    ledcWrite(0, (int)(p*255));

    int value = s.read();
    Serial.println("value: " + String(value) );

    vTaskDelay(200);
  }
}

void setup() {

  Serial.begin(115200);

  while (!Serial) {}

  ledcAttachPin(32, 0);  // servo_output on pin 32
  ledcSetup(0, 50, 8);
  ledcWrite(0, 128);

  pinMode(36, INPUT); // servo_input on pin 36
  s.begin(36);

  pinMode(39, INPUT); // potentiometer on pin 39
  pot.begin(39);

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
