#ifndef IMU_BNO055_h
#define IMU_BNO055_h
#include "config.h"

#ifdef USE_IMU_BNO055

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>



// make the BNO055 object accessible when module is included
extern Adafruit_BNO055 bno;

extern sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData, gravityData;
extern uint8_t imuTemp, imuSystemCal, imuGyroCal, imuAccelCal, imuMagCal;
extern SemaphoreHandle_t imuData_lock;
extern TimerHandle_t imuUpdateTimer;


void imuUpdateServiceWrapper(TimerHandle_t xTimer);
void imuUpdateHelper();



#endif
#endif