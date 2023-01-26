#include "config.h"

#ifdef USE_IMU_BNO055
#include "IMU_BNO055.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// 55 is an id for adafruit_sensor
// current module has addrexx 0x29 but others may have 0x28
// may need to be changed
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);
sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData, gravityData;
uint8_t imuTemp, imuSystemCal, imuGyroCal, imuAccelCal, imuMagCal;
SemaphoreHandle_t imuData_lock = xSemaphoreCreateMutex();  // create mutex
TimerHandle_t imuUpdateTimer = xTimerCreate("IMU Service", pdMS_TO_TICKS(BNO055_SAMPLERATE_DELAY_MS), pdTRUE, 0, imuUpdateServiceWrapper);

void imuUpdateServiceWrapper(TimerHandle_t xTimer){
  imuUpdateHelper();
}

void imuUpdateHelper() {
  sensors_event_t orientationDataLocal, angVelocityDataLocal, linearAccelDataLocal, magnetometerDataLocal, accelerometerDataLocal, gravityDataLocal;
  uint8_t imuTempLocal, imuSystemCalLocal, imuGyroCalLocal, imuAccelCalLocal, imuMagCalLocal;
  /*
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);
  */
  imuTempLocal = bno.getTemp();
  Serial.println(imuTempLocal);
  bno.getCalibration(&imuSystemCalLocal, &imuGyroCalLocal, &imuAccelCalLocal, &imuMagCalLocal);
  /*
  if (xSemaphoreTake(imuData_lock, BNO055_SAMPLERATE_DELAY_MS) == pdTRUE) {  // get lock on imu data variables
    orientationData = orientationDataLocal;
    angVelocityData = angVelocityDataLocal;
    linearAccelData = linearAccelDataLocal;
    magnetometerData = magnetometerDataLocal;
    accelerometerData = accelerometerDataLocal;
    gravityData = gravityDataLocal;
    imuTemp = imuTempLocal;
    imuSystemCal = imuSystemCalLocal;
    imuGyroCal = imuGyroCalLocal;
    imuAccelCal = imuAccelCalLocal;
    imuMagCal = imuMagCalLocal;    // copy local variable to global
    xSemaphoreGive(imuData_lock);  // release lock on imu data variables
  }
  */
}




#endif