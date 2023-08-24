#include "config.h"
#include "LCD.h"
#include "iron_tips.h"
#define DISPLAY_LOGO "      ALab"



void DSPL::init(void) {
  LiquidCrystal::begin(16, 2);
  LiquidCrystal::clear();
  full_second_line = false;
  LiquidCrystal::setCursor(6, 0);
  LiquidCrystal::print(F(DISPLAY_LOGO));
  delay(1500);
  LiquidCrystal::clear();
}

void DSPL::tip(const char *tip_name, bool top) {
  uint8_t  y = 1; if (top) y = 0;
  LiquidCrystal::setCursor(0, y);
  LiquidCrystal::print(tip_name);
  for (uint8_t i = strlen(tip_name); i < 16; ++i)
    LiquidCrystal::print(' ');
}

void DSPL::msgSelectTip(void) {
  LiquidCrystal::setCursor(0, 0);
  LiquidCrystal::print(F("TIP:            "));
}

void DSPL::msgActivateTip(void) {
  LiquidCrystal::setCursor(0, 0);
  LiquidCrystal::print(F("activate tip    "));
}

void DSPL::tSet(uint16_t t, bool celsius) {
  char buff[5];
  char units = 'C';
  if (!celsius) units = 'F';
  LiquidCrystal::setCursor(0, 0);
  LiquidCrystal::print("Set:");
  LiquidCrystal::setCursor(4, 0);
  sprintf(buff, "%3d%c", t, units);
  LiquidCrystal::print(buff);
}

void DSPL::tCurr(uint16_t t) {
  char buff[8];
  LiquidCrystal::setCursor(0, 1);
  if (t < 1000) {
    sprintf(buff, "Act:%3d", t);
  } else {
    LiquidCrystal::print(F("xxx"));
    return;
  }
  LiquidCrystal::print(buff);
  if (full_second_line) {
    LiquidCrystal::print(F("            "));
    full_second_line = false;
  }
}

void DSPL::pSet(uint8_t  p) {
  char buff[6];
  sprintf(buff, "P:%3d", p);
  LiquidCrystal::setCursor(0, 0);
  LiquidCrystal::print(buff);
}

void DSPL::tRef(uint8_t ref) {
  char buff[12];
  LiquidCrystal::setCursor(0, 0);
  sprintf(buff, "Ref. #%1d  ", ref + 1);
  LiquidCrystal::print(buff);
}

void DSPL::timeToOff(uint8_t  sec) {
  char buff[5];
  sprintf(buff, " %3d", sec);
  LiquidCrystal::setCursor(8, 0);
  LiquidCrystal::print(buff);
  LiquidCrystal::print(F("    "));
}

void DSPL::msgReady(void) {
  LiquidCrystal::setCursor(8, 0);
  LiquidCrystal::print(F("   READY"));
}

void DSPL::msgOn(void) {
  LiquidCrystal::setCursor(8, 0);
  LiquidCrystal::print(F("      ON"));
}

void DSPL::msgOff(void) {
  LiquidCrystal::setCursor(8, 0);
  LiquidCrystal::print(F("     OFF"));
}

void DSPL::msgStandby(void) {
  LiquidCrystal::setCursor(8, 0);
  LiquidCrystal::print(F("   SLEEP"));
}

void DSPL::msgCold(void) {
  LiquidCrystal::setCursor(0, 1);
  LiquidCrystal::print(F("        COLD"));
  full_second_line = true;
}

void DSPL::msgFail(void) {
  LiquidCrystal::setCursor(0, 1);
  LiquidCrystal::print(F("   Failed  "));
}
void DSPL::msgTune(void) {
  LiquidCrystal::setCursor(0, 0);
  LiquidCrystal::print(F("Tune"));
}



void DSPL::msgDefault() {
  LiquidCrystal::setCursor(0, 1);
  LiquidCrystal::print(F(" default        "));
}

void DSPL::setupMode(uint8_t mode, bool tune, uint16_t p) {
  char buff[5];
  LiquidCrystal::clear();
  if (!tune) {
    LiquidCrystal::print(F("Setup"));
    LiquidCrystal::setCursor(1, 1);
  }
  switch (mode) {
    case 0:                                 // C/F. In-line editing
      LiquidCrystal::print(F("Units"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);//7.1
        if (p)
          LiquidCrystal::print(F("Celsius"));
        else
          LiquidCrystal::print(F("Fahrenheit"));
      }
      break;
    case 1:                                 // Buzzer
      LiquidCrystal::print(F("Buzzer"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);
        if (p)
          LiquidCrystal::print(F("ON"));
        else
          LiquidCrystal::print(F("OFF"));
      }
      break;
    case 2:                                 // Switch type
      LiquidCrystal::print(F("Switch"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);
        if (p)
          LiquidCrystal::print(F("REED"));
        else
          LiquidCrystal::print(F("TILT"));
      }
      break;
    case 3:                                 // ambient temperatyre sensor
      LiquidCrystal::print(F("Ambient"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);
        if (p)
          LiquidCrystal::print(F("ON"));
        else
          LiquidCrystal::print(F("OFF"));
      }
      break;
    case 4:                                 // standby temperature
      LiquidCrystal::print(F("Stby Temp"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);
        if (p > 0) {
          sprintf(buff, "%3d", p);
          LiquidCrystal::print(buff);
        } else {
          LiquidCrystal::print(" NO");
        }
      }
      break;
    case 5:                                 // Standby Time
      LiquidCrystal::print(F("Stby Time"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);
        sprintf(buff, "%3ds", p);
        LiquidCrystal::print(buff);
      }
      break;
    case 6:                                 // off-timeout
      LiquidCrystal::print(F("Auto OFF"));
      if (tune) {
        LiquidCrystal::setCursor(1, 1);
        if (p > 0) {
          sprintf(buff, "%2dm", p);
          LiquidCrystal::print(buff);
        } else {
          LiquidCrystal::print(" NO");
        }
      }
      break;
    case 7:                                 // Tip calibrage
      LiquidCrystal::print(F("tip config."));
      break;
    case 8:                                 // Activate tip
      LiquidCrystal::print(F("activate tip"));
      break;
    case 9:                                 // Tune controller
      LiquidCrystal::print(F("TUNE"));
      break;
    case 10:                                // save
      LiquidCrystal::print(F("APPLY"));
      break;
    case 11:                                // cancel
      LiquidCrystal::print(F("CANCEL"));
    default:
      break;
  }
}

void DSPL::percent(uint8_t  Power) {
  char buff[6];
  sprintf(buff, "%3d%c", Power, '%');
  LiquidCrystal::setCursor(12, 1);
  LiquidCrystal::print(buff);
}

void DSPL::mark(char sym, bool on) {
  char buff[5];
  for (uint8_t  i = 0; i < 4; ++i) buff[i] = ' ';
  if (on) buff[3] = sym;
  buff[4] = '\0';
  LiquidCrystal::setCursor(4, 1);
  LiquidCrystal::print(buff);
}

void DSPL::calibrated(bool calibrated) {
  char buff[5];
  for (uint8_t  i = 0; i < 4; ++i) buff[i] = ' ';
  if (!calibrated) buff[3] = '*';
  buff[4] = '\0';
  LiquidCrystal::setCursor(4, 1);
  LiquidCrystal::print(buff);
}
