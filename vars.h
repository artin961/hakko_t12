#ifndef _VARS_H
#define _VARS_H
#include <Arduino.h>

extern const uint16_t default_temperature;
extern const uint16_t temp_max;
extern const uint16_t temp_minC;
extern const uint16_t temp_maxC;
extern const uint16_t temp_minF;
extern const uint16_t temp_maxF;
extern const uint16_t temp_tip[3];
extern const uint16_t ambient_tempC;
extern const uint16_t ambient_tempF;

// The variables for Timer1 operations
extern volatile uint16_t tmr1_count;  // The count to calculate the temperature and the current check periods
extern volatile bool iron_off;        // Whether the IRON is switched off to check the temperature
extern volatile uint8_t tilt_ints;

extern boolean led;
extern const uint32_t temp_check_period;  // The IRON temperature check period, ms
#endif
