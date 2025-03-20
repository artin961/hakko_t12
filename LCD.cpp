#include "LCD.h"
const char *modes[] = {
  "Units", "Buzzer", "Switch", "Ambient", "Stby Temp", "Stby Time",
  "Auto OFF", "Tip config.", "Activate tip", "TUNE", "APPLY", "CANCEL"
};

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
void DSPL::tSet(uint16_t t, bool celsius)
{
  LiquidCrystal::setCursor(0, 0);
  char buff[10]; // Ensure space for null-terminator
  sprintf(buff, "Set:%3d%c", t, celsius ? 'C' : 'F');
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

void DSPL::msgStateMsg(const char* message) {
  char buffer[9]; // Buffer for 8 characters + null terminator
  int len = strlen(message);

  // Calculate leading spaces
  int spaces = 8 - len;
  if (spaces < 0) spaces = 0; // Prevent negative values

  // Fill the buffer with spaces
  memset(buffer, ' ', spaces);

  // Copy the message after the leading spaces
  strncpy(buffer + spaces, message, 8 - spaces);

  buffer[8] = '\0'; // Null-terminate the string

  LiquidCrystal::setCursor(8, 0); // Start printing from (8,0)
  LiquidCrystal::print(buffer); // Print to LCD
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

void DSPL::setupMode(uint8_t mode, bool tune, uint16_t p, bool celsius)
{
  LiquidCrystal::clear();
  if (mode > 11)
    return; // Prevent out-of-bounds access

  LiquidCrystal::print(tune ?  modes[mode] : "Setup");
  LiquidCrystal::setCursor(1, 1);

  if (tune)
  {

    char buff[6];
    switch (mode)
    {
      case 0:
        LiquidCrystal::print(p ? "Celsius" : "Fahrenheit");
        break;
      case 1:
      case 3:
        LiquidCrystal::print(p ? "ON" : "OFF");
        break;
      case 2:
        LiquidCrystal::print(p ? "REED" : "TILT");
        break;
      case 4:
      case 5:
      case 6:
        sprintf(buff, "%3d%c", p, (mode == 6) ? 'm' : ((mode == 5) ? 's' : (celsius ? 'C' : 'F')));
        LiquidCrystal::print(buff);
        break;
    }
  }
  else
    LiquidCrystal::print(modes[mode]);
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
