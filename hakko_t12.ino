/*
  Soldering IRON controller for hakko t12 tips built on atmega328 microcontroller running 16 MHz
  The controller is using interrupts from the Timer1 to generate high-frequence PWM signal on port D10
  to silently heat the IRON and periodically check the IRON temperature on overflow interrupts
  Timer1 runs with prescale 1 through 0 to 255 and back and its frequency is 31250 Hz.
  The owerflow interrupt running as folows:
  First, the current through the IRON is checked
  then the power, supplien to the IRON interrupted and the controller waits for 32 timer interrupts (about 1 ms)
  then the IRON temperature is checked and the power to the IRON restored
  then the controller waits for check_period Timer1 interrupts to restart the all procedure over again
*/
//#define DEBUGEEE
// Edit the configuration file to select appropriate display type
#include <EEPROM.h>
#include "config.h"
#include "encoder.h"
#include "cfg.h"
#include "iron_tips.h"
#include "vars.h"
#include <avr/wdt.h> /* Header for watchdog timers in AVR */

#include "Buzzer.h"
#include "LCD.h"
#include "Iron.h"
#include "Screen.h"

DSPL disp;
RENC encoder;
IRON iron(ADC_THERMOCOUPLE, ADC_CURRENT, ADC_THERMISTOR, ADC_TILT_REED);
IRON_CFG ironCfg(MAX_CUSTOM_TIPS);  // See config.h
BUZZER simpleBuzzer;
TIPS tips;

mainSCREEN offScr(&iron, &disp, &encoder, &simpleBuzzer, &ironCfg);
tipSCREEN selScr(&iron, &disp, &encoder, &ironCfg);
workSCREEN wrkScr(&iron, &disp, &encoder, &simpleBuzzer, &ironCfg);
errorSCREEN errScr(&iron, &disp, &simpleBuzzer);
powerSCREEN pwrScr(&iron, &disp, &encoder, &ironCfg);
configSCREEN cfgScr(&iron, &disp, &encoder, &ironCfg, &simpleBuzzer);
calibSCREEN tipScr(&iron, &disp, &encoder, &ironCfg, &simpleBuzzer);
actSCREEN actScr(&iron, &disp, &encoder, &ironCfg);
tuneSCREEN tuneScr(&iron, &disp, &encoder, &simpleBuzzer);
//pidSCREEN		pidScr(&iron, &encoder);

SCREEN* pCurrentScreen = &offScr;
//SCREEN *pCurrentScreen = &pidScr;

/*
   The timer1 overflow interrupt handler.
   Activates the procedure for IRON current check or for IRON temperature check
   Interrupt routine on Timer1 overflow, @31250 Hz, 32 microseconds is a timer period
   keepTemp() function takes 353 mks, about 12 ticks of TIMER1;
   We should wait for 33 timer ticks before checking the temperature after iron was powered off
*/
const uint32_t period_ticks = (31250 * temp_check_period) / 1000 - 33 - 12;
ISR(TIMER1_OVF_vect) {
  if (iron_off) {              // The IRON is switched off, we need to check the temperature
    if (++tmr1_count >= 33) {  // about 1 millisecond
      TIMSK1 &= ~_BV(TOIE1);   // disable the overflow interrupts
      iron.keepTemp();         // Check the temp. If on, keep the temperature
      tmr1_count = 0;
      iron_off = false;
      TIMSK1 |= _BV(TOIE1);  // enable the the overflow interrupts
    }
  } else {  // The IRON is on, check the current and switch-off the IRON
    if (++tmr1_count >= period_ticks) {
      TIMSK1 &= ~_BV(TOIE1);  // disable the overflow interrupts
      tmr1_count = 0;
      OCR1B = 0;  // Switch-off the power to check the temperature
      HEATER_PORT &= ~HEATER_BITMASK;
      iron_off = true;
      TIMSK1 |= _BV(TOIE1);  // enable the overflow interrupts
    }
  }
}

// the setup routine runs once when you press reset:
void setup() {
  wdt_reset();         /* Reset the watchdog */
  wdt_enable(WDTO_2S); /* Enable the watchdog with a timeout of 2 seconds */
#ifdef DEBUGEEE
  Serial.begin(115200);
#endif

  pinMode(lcdled_PIN, OUTPUT);
  pinMode(ledPIN, OUTPUT);
  digitalWrite(ledPIN, HIGH);
  digitalWrite(lcdled_PIN, HIGH);
  disp.init();
  // Load configuration parameters
  ironCfg.init();
  iron.init();
  uint16_t temp = ironCfg.tempPresetHuman();
  int16_t ambient = 0;
  for (uint8_t i = 0; i < 10; ++i) {
    int16_t amb = iron.ambientTemp();
    if (amb == ambient) break;
    wdt_reset(); /* Reset the watchdog */
    delay(500);
    ambient = amb;
  }
  temp = ironCfg.humanToTemp(temp, ambient);
  iron.setTemp(temp);
  simpleBuzzer.activate(ironCfg.isBuzzer());

  // Initialize rotary encoder
  encoder.init();
  wdt_reset(); /* Reset the watchdog */
  delay(500);
  //attachInterrupt(digitalPinToInterrupt(R_MAIN_PIN), rotEncChange, CHANGE);

  // Initialize SCREEN hierarchy
  offScr.next = &wrkScr;
  offScr.nextL = &cfgScr;
  offScr.no_iron = &selScr;

  wrkScr.next = &offScr;
  wrkScr.nextL = &pwrScr;
  wrkScr.main = &offScr;

  errScr.next = &offScr;
  errScr.nextL = &offScr;
  pwrScr.nextL = &wrkScr;
  pwrScr.no_iron = &errScr;
  cfgScr.next = &tuneScr;
  cfgScr.nextL = &offScr;
  cfgScr.main = &offScr;
  cfgScr.calib = &tipScr;
  cfgScr.activate = &actScr;
  tipScr.nextL = &offScr;
  tuneScr.next = &cfgScr;
  tuneScr.main = &offScr;
  selScr.nextL = &cfgScr;
  selScr.no_iron = &offScr;
  actScr.main = &offScr;
  actScr.nextL = &offScr;
  pCurrentScreen->init();
  wdt_reset(); /* Reset the watchdog */
}

// Encoder interrupt handler
ISR(INT1_vect) {
  encoder.encoderIntr();
}

// The main loop
void loop() {
  wdt_reset(); /* Reset the watchdog */
  static int16_t old_pos = encoder.read();

  SCREEN* nxt = pCurrentScreen->returnToMain();
  if (nxt != pCurrentScreen) {  // return to the main screen by timeout
    pCurrentScreen = nxt;
    pCurrentScreen->init();
  }

  int16_t pos = encoder.read();
  if (old_pos != pos) {
    pCurrentScreen->rotaryValue(pos);
    old_pos = pos;
    if (pCurrentScreen->isSetup())
      pCurrentScreen->resetTimeout();
  }

  uint8_t bStatus = encoder.buttonCheck();
  switch (bStatus) {
    case 2:  // int32_t press;
      nxt = pCurrentScreen->menu_long();
      if (nxt != pCurrentScreen) {
        pCurrentScreen = nxt;
        pCurrentScreen->init();
      } else {
        if (pCurrentScreen->isSetup())
          pCurrentScreen->resetTimeout();
      }
      break;
    case 1:  // short press
      nxt = pCurrentScreen->menu();
      if (nxt != pCurrentScreen) {
        pCurrentScreen = nxt;
        pCurrentScreen->init();
      } else {
        if (pCurrentScreen->isSetup())
          pCurrentScreen->resetTimeout();
      }
      break;
    case 0:  // Not pressed
    default:
      break;
  }

  nxt = pCurrentScreen->show();
  if (nxt && pCurrentScreen != nxt) {  // Be paranoiac, the returned value must not be null
    pCurrentScreen = nxt;
    pCurrentScreen->init();
  }

  iron.checkSWStatus();
}
