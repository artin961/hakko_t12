#include "vars.h"

const uint16_t default_temperature = 300;       // Default value of IRON temperature
const uint16_t temp_max  = 950;                 // Maximum possible temperature in internal units
const uint16_t temp_minC = 150;                 // Minimum calibration temperature in degrees of Celsius
const uint16_t temp_maxC = 450;                 // Maximum calibration temperature in degrees of Celsius
const uint16_t temp_minF = (temp_minC *9 + 32*5 + 2)/5;
const uint16_t temp_maxF = (temp_maxC *9 + 32*5 + 2)/5;
const uint16_t temp_tip[3] = {200, 300, 400};   // Reference temperatures for tip calibration (Celsius)
const uint16_t ambient_tempC = 25;              // Default ambient temperature in Celsius
const uint16_t ambient_tempF = (ambient_tempC *9 + 32*5 + 2)/5;


extern volatile uint16_t tmr1_count=0;  // The count to calculate the temperature and the current check periods
extern volatile bool iron_off=false;        // Whether the IRON is switched off to check the temperature
extern volatile uint8_t tilt_ints=0;

extern boolean led = false;
extern const uint32_t temp_check_period = 50;  // The IRON temperature check period, ms
