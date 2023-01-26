// config options
// comment lines out to disable features/modules

#ifndef config_h
#define config_h

/*
* comment to disable gps
*/

#define USE_GPS

/*
* comment to disable temperature sensing thru the MCP9808 module
* eventually will be swapped out for thermistors
*/

#define USE_TEMP_MCP9808

/*
* use the BNO055 IMU
* potentially will add additional sensors
*/

#define USE_IMU_BNO055

/* Set the delay between fresh samples */

#define BNO055_SAMPLERATE_DELAY_MS 1000

#endif