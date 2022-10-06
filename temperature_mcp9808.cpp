#include "config.h"
#ifdef USE_TEMP_MCP9808

// temp sensing logic
// currently uses an MCP9808 I2C temp sensor and has basic cutoff logic
// will be replaced with thermistors
#include "temperature_mcp9808.h"
#include <Wire.h>
#include "Adafruit_MCP9808.h"

float tempC = 0;
SemaphoreHandle_t tempC_lock = xSemaphoreCreateMutex();  // create mutex
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setLED(void *parameter) {
  float tempC_local = 0;
  bool ledPowerOn = false;
  while (1) {
    if (xSemaphoreTake(tempC_lock, 0) == pdTRUE) {  // get lock on tempC
      tempC_local = tempC;                          // copy global variable to local
      xSemaphoreGive(tempC_lock);                   // release lock on tempC
    }
    ledPowerOn = tempC_local > tempLimit;  // turn on LED if temperature is above threshold

    // logic for when temp is above threshold
    // likely will be replaced by motor shutdown logic in future

    if (ledPowerOn) {
      digitalWrite(led_pin, HIGH);
      Serial.print("Blinking led because temperature is ");
      Serial.print(tempC_local, 4);
      Serial.print("*C, ");
      Serial.print("which exceeds the limit of ");
      Serial.print(tempLimit);
      Serial.println("*C");
    } else {
      digitalWrite(led_pin, LOW);
      Serial.print("Temperature is at ");
      Serial.print(tempC_local, 4);
      Serial.print("*C, ");
      Serial.print("which is below the limit of ");
      Serial.print(tempLimit);
      Serial.println("*C");
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}


void readTempC(void *parameter) {
  while (1) {
    tempsensor.wake();  // wake up, ready to read!

    // read temp
    float c = tempsensor.readTempC();
    float f = tempsensor.readTempF();


    Serial.print("Temp: ");
    Serial.print(c, 4);
    Serial.print("*C\t and ");
    Serial.print(f, 4);
    Serial.println("*F.");
    if (xSemaphoreTake(tempC_lock, 0) == pdTRUE) {  // get lock on tempC
      tempC = c;                                    // copy measurement to global variable
      Serial.println("writing temperature");
      xSemaphoreGive(tempC_lock);  // release lock on tempC
    }

    tempsensor.shutdown_wake(1);  // shutdown MCP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

#endif