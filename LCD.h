#ifndef _DSPL1602_H_
#define _DSPL1602_H_
#include "Config.h"
#include "LiquidCrystal.h"
#include "iron_tips.h"

class DSPL : protected LiquidCrystal
{
  public:
    DSPL(void) : LiquidCrystal() {}
    void init(void);
    void backlight(void) {
      LiquidCrystal::backlight();
    }
    void noBacklight(void) {
      LiquidCrystal::noBacklight();
    }
    void clear(void)
    {
      LiquidCrystal::clear();
    }
    void tip(const char *tip_name, bool top); // Show the current tip (on top line)
    void tSet(uint16_t t, bool celsuis);      // Show the temperature set
    void tCurr(uint16_t t);                   // Show The current temperature
    void pSet(uint8_t p);                     // Show the power set
    void tRef(uint8_t ref);                   // Show the reference temperature index
    void timeToOff(uint8_t sec);              // Show the time to automatic off the IRON
    void msgStateMsg(const char* message, uint8_t row = 0);
    void msgReady(void) {
      msgStateMsg("ON"); // Show 'Ready' message
    }
    void msgOn(void) {
      msgStateMsg("ON"); // Show 'On' message
    }
    void msgOff(void) {
      msgStateMsg("OFF"); // Show 'Off' message
    }
    void msgStandby(void) {
      msgStateMsg("SLEEP"); // Show 'stb' message
    }
    void msgCold(void);                                      // Show 'Cold' message
    void msgFail(void);                                      // Show 'Fail' message
    void msgTune(void);                                      // Show 'Tune' message
    void msgDefault();                                       // Show 'default' message (load default configuration)
    void msgSelectTip(void);                                 // Show 'iron tip' message
    void msgActivateTip(void);                               // Show 'act. tip' message
    void setupMode(uint8_t mode, bool tune, uint16_t p = 0,  bool celsuis = true); // Show the configuration mode [0 - 11]
    void percent(uint8_t Power);                             // Show the percentage
    void mark(char sym, bool on);                            // Show mark sign if true
    void calibrated(bool calibrated);                        // Show '*' if the tip was not calibrated
  private:
};
#endif
