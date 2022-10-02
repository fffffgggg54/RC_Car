// for now this is a placeholder for the main program.
// currently uses an MCP9808 I2C temp sensor and has basic cutoff logic
// will be replaced with thermistors



#include <Wire.h>
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

float tempLimit = 30; // temperature limit in degrees C
static const int led_pin = 2;
static float tempC = 0;
static SemaphoreHandle_t tempC_lock;

void setLED(void *parameter){
  float tempC_local = 0;
  bool ledPowerOn = false;
  while(1){
    if(xSemaphoreTake(tempC_lock, 0) == pdTRUE){
      tempC_local = tempC;
      Serial.println(tempC);
      Serial.println(tempC_local, 4);
      xSemaphoreGive(tempC_lock);
      Serial.println("got value");
    }
    ledPowerOn = tempC_local > tempLimit;


    if(ledPowerOn) {
      digitalWrite(led_pin, HIGH);
      Serial.print("Blinking led because temperature is "); Serial.print(tempC_local, 4); Serial.print("*C, ");
      Serial.print("which exceeds the limit of "); Serial.print(tempLimit); Serial.println("*C");
    }
    else {
      digitalWrite(led_pin, LOW);
      Serial.print("Temperature is at "); Serial.print(tempC_local, 4); Serial.print("*C, ");
      Serial.print("which is below the limit of "); Serial.print(tempLimit); Serial.println("*C");
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}


void readTempC(void *parameter){
  while(1){
    tempsensor.wake();   // wake up, ready to read!


    float c = tempsensor.readTempC();
    float f = tempsensor.readTempF();

    
    Serial.print("Temp: "); 
    Serial.print(c, 4); Serial.print("*C\t and "); 
    Serial.print(f, 4); Serial.println("*F.");
    if(xSemaphoreTake(tempC_lock, 0) == pdTRUE){
      tempC = c;
      Serial.println("writing temperature");
      xSemaphoreGive(tempC_lock);
    }

    tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling

    vTaskDelay(500/portTICK_PERIOD_MS);



  }


}

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  while (!Serial); //waits for serial terminal to be open, necessary in newer arduino boards.
  Serial.println("MCP9808 demo");
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example, also can be left in blank for default address use
  // Also there is a table with all addres possible for this sensor, you can connect multiple sensors
  // to the same i2c bus, just configure each sensor with a different address and define multiple objects for that
  //  A2 A1 A0 address
  //  0  0  0   0x18  this is the default address
  //  0  0  1   0x19
  //  0  1  0   0x1A
  //  0  1  1   0x1B
  //  1  0  0   0x1C
  //  1  0  1   0x1D
  //  1  1  0   0x1E
  //  1  1  1   0x1F
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
    
   Serial.println("Found MCP9808!");

  tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
  tempC_lock = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(setLED,
              "Conditional LED Blink",
              1024,
              NULL,
              1,
              NULL,
              1);
  xTaskCreatePinnedToCore(readTempC,
              "Temperature Sensor Read",
              2048,
              NULL,
              1,
              NULL,
              1);

}

void loop() {
  
}
