#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
// Select appropriate display type

#include "LCD.h"

/*
 * Maximum supported custom tips for controller, see iron_tips.h and cfg.h
 * Each custom tip data requires 12 bytes in EEPROM, 504 bytes per 42 tips are required.
 * 
 */
#define MAX_CUSTOM_TIPS (30)

#endif
