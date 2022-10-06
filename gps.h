#ifndef gps_h
#define gps_h
#include "config.h"

#ifdef USE_GPS

#include <TinyGPSPlus.h>

static const uint32_t GPSBaud = 115200;  // gps configured to run at 115200 baud

// make the gps object accessible when module is included
extern TinyGPSPlus gps;

// parse data in serial buffer
void gpsUpdate(void *parameter);

// do something with gps data
void gpsReadData(void *parameter);

// various helper functions for printing gps data
void printFloat(float val, bool valid, int len, int prec);
void printInt(unsigned long val, bool valid, int len);
void printDateTime(TinyGPSDate &d, TinyGPSTime &t);
void printStr(const char *str, int len);



#endif
#endif