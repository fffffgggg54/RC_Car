#include "config.h"
#ifdef USE_SERVO_LOGGING

// temporary, only logs for one servo

uint32_t servoTick_rising = 0;
uint32_t servoTick_falling = 0;
float servoValue = -1;

SemaphoreHandle_t servoTick_lock = xSemaphoreCreateMutex();

void int_RISING(void* param){
  
  if(xSemaphoreTakeFromISR(servoTick_lock) == pdTRUE){
    
    servoTick_rising = xTaskGetTickCountFromISR();
    xSemaphoreGiveFromISR(servoTick_lock);

  }
}

void int_FALLING(void* param){

  if(xSemaphoreTakeFromISR(servoTick_lock) == pdTRUE){

    int width_HIGH = -1;
    int width_LOW = -1;

    width_LOW = servoTick_rising - servoTick_falling; // use servoTick_falling form previous interrupt cycle

    servoTick_falling = xTaskGetTickCountFromISR();

    width_HIGH = servoTick_falling - servoTick_rising;

    servoValue = width_HIGH / (width_LOW + width_HIGH);

    xSemaphoreGiveFromISR(servoTick_lock);

    Serial.print("[servo X] Value: ");
    Serial.print(value);
    Serial.print("Angle: ");
    Serial.print(value * 180);
    Serial.println("°");

  }
}

#endif
