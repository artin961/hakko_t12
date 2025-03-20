#ifndef _DSPL1602_H_
#define _DSPL1602_H_
#include "config.h"
#include "LiquidCrystal.h"
#include "iron_tips.h"

// The LCD 0802 parallel interface
const uint8_t LCD_RS_PIN = 13;
const uint8_t LCD_E_PIN = 12;
const uint8_t LCD_DB4_PIN = 5;
const uint8_t LCD_DB5_PIN = 6;
const uint8_t LCD_DB6_PIN = 7;
const uint8_t LCD_DB7_PIN = 8;

class DSPL : protected LiquidCrystal
{
  public:
    DSPL(void) : LiquidCrystal(LCD_RS_PIN, LCD_E_PIN, LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN) {}
    void init(void);
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
    void msgStateMsg(const char* message);
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
    bool full_second_line; // Whether the second line is full with the message
};
#endif
