// config options
// comment lines out to disable features

#define USE_GPS

#define USE_TEMP_MCP9808

#ifdef USE_GPS

#include <TinyGPSPlus.h>

static const uint32_t GPSBaud = 115200;  // gps configured to run at 115200 baud

// The TinyGPSPlus object
TinyGPSPlus gps;


void gpsUpdate(void *parameter) {
  while (1) {
    if (Serial2.available()) {       // check if there is incoming data
      while (Serial2.available()) {  // if there is incoming data...
        gps.encode(Serial2.read());  // ...use the gps object to parse all of it
      }
      // after reading all of the messages,
      // send a signal to the task with the name "GPSOBJRead" which, in this case, is gpsReadData()
      // the signal unblocks the task and tells it to run whatever should run after getting fresh gps data
      xTaskNotifyGive(xTaskGetHandle("GPSOBJRead"));
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // wait a bit, since the gps won't be sending anything for a while
  }
}

/*
* function to do something with the gps object after gpsUpdate()
* signals that it has been updated with fresh data.
* it currently prints out a bunch of values
*/

void gpsReadData(void *parameter) {
  // print column labels
  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
  while (1) {
    ulTaskNotifyTake(pdTRUE, 5000);  // wait for signal from gpsUpdate() for 5000ms
    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    // print a bunch of stuff

    printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
    printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    printInt(gps.location.age(), gps.location.isValid(), 5);
    printDateTime(gps.date, gps.time);
    printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
    printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
    printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
    printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

    unsigned long distanceKmToLondon =
      (unsigned long)TinyGPSPlus::distanceBetween(
        gps.location.lat(),
        gps.location.lng(),
        LONDON_LAT,
        LONDON_LON)
      / 1000;
    printInt(distanceKmToLondon, gps.location.isValid(), 9);

    double courseToLondon =
      TinyGPSPlus::courseTo(
        gps.location.lat(),
        gps.location.lng(),
        LONDON_LAT,
        LONDON_LON);

    printFloat(courseToLondon, gps.location.isValid(), 7, 2);

    const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

    printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

    // gps parsing diagnostics
    // sentencesWithFix() should go up while failedChecksum() should stay at 0

    printInt(gps.charsProcessed(), true, 6);
    printInt(gps.sentencesWithFix(), true, 10);
    printInt(gps.failedChecksum(), true, 9);
    Serial.println();

    if (millis() > 5000 && gps.charsProcessed() < 10)
      Serial.println(F("No GPS data received: check wiring"));
  }
}

// a bunch of helper functions for gpsReadData()

static void printFloat(float val, bool valid, int len, int prec) {
  if (!valid) {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  } else {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1);  // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                           : vi >= 10  ? 2
                                       : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
}

static void printInt(unsigned long val, bool valid, int len) {
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t) {
  if (!d.isValid()) {
    Serial.print(F("********** "));
  } else {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid()) {
    Serial.print(F("******** "));
  } else {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
}

static void printStr(const char *str, int len) {
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
}

#endif


#ifdef USE_TEMP_MCP9808

// temp sensing logic
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

void setLED(void *parameter) {
  float tempC_local = 0;
  bool ledPowerOn = false;
  while (1) {
    if (xSemaphoreTake(tempC_lock, 0) == pdTRUE) {    // get lock on tempC
      tempC_local = tempC;                            // copy global variable to local
      xSemaphoreGive(tempC_lock);                     // release lock on tempC
    }
    ledPowerOn = tempC_local > tempLimit;             // turn on LED if temperature is above threshold

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
    if (xSemaphoreTake(tempC_lock, 0) == pdTRUE) {    // get lock on tempC
      tempC = c;                                      // copy measurement to global variable
      Serial.println("writing temperature");
      xSemaphoreGive(tempC_lock);                     // release lock on tempC
    }

    tempsensor.shutdown_wake(1);  // shutdown MCP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

#endif




void setup() {




  /***********************************************
  *
  *       BASIC SETUP
  *
  ***********************************************/

  Serial.begin(115200);
  while (!Serial)
    ;

    /***********************************************
  *
  *       DEVICE SPECIFIC SETUP
  *
  ***********************************************/


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
  tempC_lock = xSemaphoreCreateMutex();  // create mutex
  xTaskCreate(setLED,                    // function to call
              "Conditional LED Blink",   // task name
              1024,                      // task stack size (bytes)
              NULL,                      // parameters to pass
              1,                         // task priority, 0 to configMAX_PRIORITIES-1
              NULL);                     // task handle
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