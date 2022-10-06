#include "config.h"
#include "gps.h"
#include "temperature_mcp9808.h"


void setup() {
  // basic setup

  Serial.begin(115200);
  while (!Serial) {}

  // go through the devices, performing the necesary setup and creating the tasks if config.h says to


#ifdef USE_TEMP_MCP9808
  pinMode(led_pin, OUTPUT);

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
    while (1)
      ;
  }

  Serial.println("Found MCP9808!");

  tempsensor.setResolution(3);  // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  xTaskCreate(setLED,                   // function to call
              "Conditional LED Blink",  // task name
              1024,                     // task stack size (bytes)
              NULL,                     // parameters to pass
              1,                        // task priority, 0 to configMAX_PRIORITIES-1
              NULL);                    // task handle
  xTaskCreate(readTempC,
              "Temperature Sensor Read",
              2048,
              NULL,
              1,
              NULL);

#endif



#ifdef USE_GPS
  // initialize Serial2 for use with GPS
  // softwareserial doesn't work with 115200 and a buffer of 64 bytes and a report rate of 10hz
  // if needed, the module can be reconfigured to operate at a lower baud rate and report rate
  // allowing use of softwareserial
  Serial2.begin(GPSBaud);

  // task to periodically check if there is new data from the gps and if so, parse it using the gps object
  xTaskCreate(gpsUpdate,
              "GPSParse",
              2048,
              NULL,
              2,
              NULL);
  // task to handle fresh data in the gps object after reading from it
  xTaskCreate(gpsReadData,
              "GPSOBJRead",
              2048,
              NULL,
              1,
              NULL);

#endif
}

void loop() {}