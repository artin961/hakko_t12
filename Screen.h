#ifndef _SCREEN_H_
#define _SCREEN_H_
#include <Arduino.h>
#include "Iron.h"
#include "LCD.h"
#include "Encoder.h"
#include "Buzzer.h"
#include "IronConfig.h"
#include "Config.h"

class SCREEN {
  public:
    SCREEN* next;     // Pointer to the next screen
    SCREEN* nextL;    // Pointer to the next Level screen, usually, setup
    SCREEN* main;     // Pointer to the main screen
    SCREEN* no_iron;  // Pointer to the screen when the IRON was disconnected
    SCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg ):
      pIron(Iron), pD(DSP), pEnc(ENC), pBz(Buzz), pCfg(Cfg) {
      update_period = CONFIG_DISPLAY_TIME;
      next = nextL = main = no_iron = 0;
      update_screen = 0;
      scr_timeout = 0;
      time_to_return = 0;
    }
    virtual void init(void) {}
    virtual SCREEN* render(uint32_t ms) = 0; // Abstract method for derived classes called when there is a need for screen update

    virtual SCREEN* show() {
      if (!shouldUpdate()) return this;
      updated();
      return this->render(millis());  // Call a virtual function for child-specific behavior
    }

    virtual SCREEN* menu(void) {
      if (this->next != 0) return this->next;
      else return this;
    }

    virtual SCREEN* menu_long(void) {
      if (this->nextL != 0) return this->nextL;
      else return this;
    }
    virtual void rotaryValue(int16_t value) {}
    bool isSetup(void) {
      return (scr_timeout != 0);
    }
    void forceRedraw(void) {
      update_screen = 0;
    }
    virtual SCREEN* returnToMain(void) {
      if (main && scr_timeout && (millis() >= time_to_return)) {
        scr_timeout = 0;
        return main;
      }
      return this;
    }
    void resetTimeout(void) {
      if (scr_timeout > 0)
        time_to_return = millis() + scr_timeout * 1000;
    }
    void setSCRtimeout(uint16_t t) {
      scr_timeout = t;
      resetTimeout();
    }
    bool wasRecentlyReset(void) {
      uint32_t to = (time_to_return - millis()) / 1000;
      return ((scr_timeout - to) < 15);
    }
  protected:
    IRON* pIron;                                 // Pointer to the IRON instance
    DSPL* pD;                                    // Pointer to the DSPLay instance
    RENC* pEnc;                                  // Pointer to the rotary encoder instance
    BUZZER* pBz;                                 // Pointer to the simple buzzer instance
    IRON_CFG* pCfg;                              // Pointer to the configuration instance
    uint16_t update_period;  // Update interval in milliseconds
    uint32_t update_screen;   // Time in ms when the screen should be updated

    uint32_t scr_timeout;     // Timeout is sec. to return to the main screen, canceling all changes
    uint32_t time_to_return;  // Time in ms to return to main screen

    void updated() {
      update_screen = millis() + update_period;
    }
    bool shouldUpdate() {
      if (millis() < update_screen) return false;
      return true;
    }
};

class mainSCREEN : public SCREEN {
  public:
    mainSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);  // Setup the preset temperature
  private:
    uint32_t clear_used_ms;                      // Time in ms when used flag should be cleared (if > 0)
    uint32_t change_display;                     // Time in ms when to switch display between preset temperature and tip name
    bool used;                                   // Whether the IRON was used (was hot)
    bool cool_notified;                          // Whether there was cold notification played
    bool show_tip;                               // Whether show the tip name instead of preset temperature
    const uint32_t cool_notify_period = 120000;  // The period to display 'cool' message (ms)
    const uint16_t show_temp = 20000;            // The period in ms to show the preset temperature
};

class tipSCREEN : public SCREEN {
  public:
    tipSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);  // Select the tip
  private:
    uint8_t old_tip;               // previous tip index
};

class actSCREEN : public SCREEN {
  public:
    actSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual SCREEN* menu(void);
    virtual void rotaryValue(int16_t value);  // Select the tip
};

class workSCREEN : public SCREEN {
  public:
    workSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);  // Change the preset temperature
    virtual SCREEN* returnToMain(void);       // Automatic power-off
  private:
    void adjustPresetTemp(void);
    void hwTimeout(uint16_t low_temp, bool tilt_active);
    bool ready;                    // Whether the IRON have reached the preset temperature
    bool lowpower_mode = false;    // Whether hardware low power mode using tilt switch
    uint32_t lowpower_time = 0;    // Time when switch to standby power mode
    uint32_t auto_off_notified;    // The time (in ms) when the automatic power-off was notified
    uint16_t tempH = 0;            // The preset temperature in human readable units
};

class powerSCREEN : public SCREEN {
  public:
    powerSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);
    virtual SCREEN* menu(void);
    virtual SCREEN* menu_long(void);
  private:
    bool on = false;                     // Whether the power of soldering IRON is on
};

class errorSCREEN : public SCREEN {
  public:
    errorSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}

    virtual void init(void) {
      pIron->switchPower(false);
      pD->clear();
      pD->msgFail();
      pBz->failedBeep();
    }
    virtual  SCREEN* render(uint32_t ms) {
      return this;
    }
};

class configSCREEN : public SCREEN {
  public:
    configSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);
    virtual SCREEN* menu(void);
    virtual SCREEN* menu_long(void);
    SCREEN* calib;     // Pointer to the calibration SCREEN
    SCREEN* activate;  // Pointer to the tip activation SCREEN
  private:
    uint8_t mode;                   // Which parameter to change
    bool tune;                      // Whether the parameter is modifying
    bool changed;                   // Whether some configuration parameter has been changed
    bool cels;                      // Current Celsius/Fahrenheit;
    bool buzzer;                    // Buzzer ON/OFF
    bool reed;                      // The hardware switch type: reed/tilt
    bool ambient;                   // The ambient temperature sensor enabled/disabled
    uint8_t off_timeout;            // Automatic switch-off timeout in minutes
    uint16_t low_temp;              // Standby temperature
    uint8_t low_timeout;            // Standby timeout
};

class calibSCREEN : public SCREEN {
  public:
    calibSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);
    virtual SCREEN* menu(void);
    virtual SCREEN* menu_long(void);
  private:
    bool calibrationOLS(uint16_t* tip, uint16_t min_temp, uint16_t max_temp);
    uint8_t closestIndex(uint16_t temp);
    void updateReference(uint8_t indx);
    uint16_t calib_temp[2][MCALIB_POINTS];  // The calibration data: real temp. [0] and temp. in internal units [1]
    uint8_t ref_temp_index = 0;             // Which temperature reference to change: [0-MCALIB_POINTS]
    uint16_t tip_temp_max = 0;              // the maximum possible tip temperature in the internal units
    uint16_t preset_temp;                   // The preset temp in human readable units
    bool cels;                              // Current Celsius/Fahrenheit;
    bool ready;                             // Ready to enter real temperature
    const uint16_t t_diff = 60;             // The adjustment could be in the interval [t_ref-t_diff; t_ref+2*t_diff]
    const uint16_t start_int_temp = 200;    // Minimal temperature in internal units, about 100 degrees Celsius
};

class tuneSCREEN : public SCREEN {
  public:
    tuneSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* menu(void);
    virtual SCREEN* menu_long(void);
    virtual SCREEN* render(uint32_t ms);
    virtual void rotaryValue(int16_t value);
  private:
    bool arm_beep;                 // Whether beep is armed
    uint8_t max_power;             // Maximum possible power to be applied
};
class pidSCREEN : public SCREEN {
  public:
    pidSCREEN( IRON* Iron, DSPL* DSP, RENC* ENC, BUZZER* Buzz, IRON_CFG* Cfg)
      : SCREEN( Iron, DSP, ENC, Buzz, Cfg) {}
    virtual void init(void);
    virtual SCREEN* menu(void);
    virtual SCREEN* menu_long(void);
    virtual SCREEN* show(void);
    virtual void rotaryValue(int16_t value);
  private:
    void showCfgInfo(void);  // show the main config information: Temp set and PID coefficients
    uint8_t mode;            // Which temperature to tune [0-3]: select, Kp, Ki, Kd
    int temp_set;
    const uint16_t period = 500;
};

#endif
