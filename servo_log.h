#include "config.h"
#ifdef USE_SERVO_LOGGING

uint32_t servoTick_rising = 0;
uint32_t servoTick_falling = 0;
extern float servoValue = -1;
extern SemaphoreHandle_t servoTick_lock;

void servo_int_RISING(void* param);
void servo_int_FALLING(void* param);
#endif
