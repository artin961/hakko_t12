#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
// Select appropriate display type
/*
   Maximum supported custom tips for controller, see iron_tips.h and cfg.h
   Each custom tip data requires 12 bytes in EEPROM, 504 bytes per 42 tips are required.

*/
#define MAX_CUSTOM_TIPS (30)
#define MCALIB_POINTS 8
#define CONFIG_DISPLAY_TIME 1000
#define CONFIG_IROM_MAX_POWER 255
#define DISPLAY_LOGO "ALab"

// FIXED HEATER CONFIG
#define HEATER_DDR DDRB
#define HEATER_PORT PORTB
#define HEATER_BITMASK 0b00000100



// FIXED ROTARY ENCODER CONFIG
#define ENCODER_DDR DDRD
#define ENCODER_PORT PORTD
#define ENCODER_PIN PIND
#define ENCODER_DATA_BITMASK 0b00010000
#define ENCODER_CLOCK_BITMASK 0b00001000
#define ENCODER_BUTTON_BITMASK 0b00000100

// FIXED ROTARY ENCODER CONFIG
#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB
#define BUZZER_BITMASK 0b00001000

// FIXED LED CONFIG
#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED_BITMASK 0b00010000

#define NTC_RESISTANCE 10000  //10k
#define NTC_AMBIENT_TEMP 25   //REFERENCE FOR THE RESISTANCE
#define NTC_BETA 3977         ///NTC BETA VALUE
#define NTC_PULLUP 10000      //10k PULLUP RESISTOR



#define BUZZER_PIN 11
#define ADC_THERMOCOUPLE A1           // Thermometer pin from soldering IRON
#define ADC_CURRENT A0           // Iron current check pin
#define ADC_THERMISTOR A2          // The thermistor pin to check ambient temperature
#define ADC_TILT_REED A3          // The tilt/reed switch pin

#define TILT_PORT PORTC
#define TILT_PIN PC3
#define TILT_INTERRUPTS_THRESHHOLD 10

#define ADC_DDR DDRC
#define ADC_PORT PORTC
#define ADC_BITMASK 0b00001111;

#define H_LENGTH 16
////TONES
#define CHIRP_1KHZ 500  ///TO GENERATE 1KHZ TONE USING CHIRP FUNCTION
#define CHIRP_2KHZ 250  ///TO GENERATE 2KHZ TONE USING CHIRP FUNCTION
#define CHIRP_4KHZ 125  ///TO GENERATE 4KHZ TONE USING CHIRP FUNCTION

//#define JBC_IRON_OR_GENERIC_K 1
#ifdef JBC_IRON_OR_GENERIC_K
    // Default tip calibration values
#define CONFIG_INERNAL_READING_200 422
#define CONFIG_INERNAL_READING_300 633
#define CONFIG_INERNAL_READING_400 844
#else
    // Default tip calibration values
#define CONFIG_INERNAL_READING_200 405
#define CONFIG_INERNAL_READING_300 608
#define CONFIG_INERNAL_READING_400 811
#endif

#define CONFIG_TILT_THR_HIGH 700
#define CONFIG_TILT_THR_LOW 300
#endif
