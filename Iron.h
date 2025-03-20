#ifndef _IRON_H_
#define _IRON_H_
#include <Arduino.h>

#include "PID.h"
#include "EmpAvarage.h"
#include "Config.h"
#include "vars.h"

class FastPWM {
  public:
    FastPWM() {}
    void init(void);
    void duty(uint8_t d) {
      OCR1B = d;
    }
    void off(void) {
      OCR1B = 0;
      HEATER_PORT &= ~HEATER_BITMASK;
    }
};

class IRON : protected PID {
  public:
    IRON(uint8_t sensor_pin, uint8_t check_pin, uint8_t ambient_pin, uint8_t tilt_pin) {
      sPIN = sensor_pin;
      cPIN = check_pin;
      aPIN = ambient_pin;
      tPIN = tilt_pin;
      h_counter = h_max_counter;
    }
    typedef enum { POWER_OFF,
                   POWER_ON,
                   POWER_FIXED,
                   POWER_COOLING
                 } PowerMode;
    void init(void);
    void switchPower(bool on);
    bool isOn(void);
    uint16_t presetTemp(void) {
      return temp_set;
    }
    uint16_t currTemp(void) {
      return h_temp.read();
    }
    uint16_t tempAverage(void) {
      return h_temp.average();
    }
    uint16_t tempDispersion(void) {
      return h_temp.dispersion();
    }
    uint16_t powerDispersion(void) {
      return h_power.dispersion();
    }
    uint8_t getMaxFixedPower(void) {
      return max_fixed_power;
    }
    int16_t changePID(uint8_t p, int k) {
      return PID::changePID(p, k);
    }


    bool checkIron(void);           // Check the IRON, return true if the iron is not connected
    void keepTemp(void);            // Keep the IRON temperature, called by Timer1 interrupt
    uint8_t appliedPower(void);     // Power applied to the IRON in percents
    void setTemp(uint16_t t);       // Set the temperature to be kept (internal units)
    void lowPowerMode(uint16_t t);  // Activate low power mode (setup temp. or 0 to return to standard mode)
    uint8_t getAvgPower(void);      // Average applied power
    void fixPower(uint8_t Power);   // Set the specified power to the the soldering IRON
    void initTempHistory(void) {
      h_counter = h_max_counter;
      h_temp.init();
      mode = POWER_OFF;
    }
    bool isCold(void) {
      return (mode == POWER_OFF);
    }
    int16_t ambientTemp(void);
    void adjust(uint16_t t);
    bool isIronTiltSwitch(bool reed);
    void checkSWStatus(void);
    uint8_t sPIN;
  private:
    FastPWM fastPWM;             // Power the iron using fast PWM through D10 pin using Timer1
    uint8_t cPIN, aPIN, tPIN;    // The sensor PIN, current check PIN, ambient temperature PIN, tilt switch PIN
    uint16_t temp_set;           // The temperature that should be kept
    uint16_t temp_low;           // Low power mode temperature
    uint8_t fix_power = 0;       // Fixed power value of the IRON (or zero if off)
    uint32_t check_iron_ms = 0;  // The time in ms when check the IRON next time
    //uint32_t last_tilt_change = 0;                           // The time in ms when check the tilt switch next time
    bool disconnected;                                       // Whether no current through the IRON (the iron disconnected)
    uint32_t check_tilt_ms = 0;                              // The time in ms when check the tilt switch next time
    int h_counter;                                           // Put the temperature and power to the history, when the counter become 0
    uint8_t applied_power = 0;                               // The applied power to the IRON, used in checkIron()
    volatile PowerMode mode = POWER_OFF;                     // Working mode of the IRON
    volatile bool chill;                                     // Whether the IRON should be cooled (preset temp is lower than current)
    HISTORY h_power;                                         // The history data of power applied values
    HISTORY h_temp;                                          // The history data of the temperature
    EMP_AVERAGE current;                                     // The average value for the current through the IRON
    bool last_tilt_state = false;                            // The average value of tilt port
    EMP_AVERAGE tilt;                                        // The average value of tilt port
    volatile EMP_AVERAGE amb_int;                            // The internal reading of ambient temperature
    bool tilt_toggle = false;                                // The tilt switch changed state
    const uint8_t max_power = CONFIG_IROM_MAX_POWER;                           // maximum power to the IRON
    const uint8_t max_fixed_power = 120;                     // Maximum power in fixed power mode
    const uint16_t min_curr = 10;                            // The minimum current value to check the IRON is connected
    const uint32_t check_period = 503;                       // Check the iron period in ms
    const uint16_t h_max_counter = 500 / temp_check_period;  // Put the history data twice a second
    const uint8_t emp_k = 2;                                 // The exponential average coefficient
    const uint8_t emp_tilt = 3;                              // The exponential average coefficient for tilt pin
    const uint16_t iron_cold = 25;                           // The internal temperature when the IRON is cold
};
#endif
