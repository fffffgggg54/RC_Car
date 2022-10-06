#ifndef temperature_mcp9808_h
#define temperature_mcp9808_h
#include "config.h"

#ifdef USE_TEMP_MCP9808
#include <Wire.h>
#include "Adafruit_MCP9808.h"

static const float tempLimit = 30;  // temperature limit in degrees C
static const int led_pin = 2;
extern float tempC;
extern SemaphoreHandle_t tempC_lock;

// Create the MCP9808 temperature sensor object
extern Adafruit_MCP9808 tempsensor;

void setLED(void *parameter);
void readTempC(void *parameter);

#endif
#endif