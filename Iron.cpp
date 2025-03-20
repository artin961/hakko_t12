#include "Iron.h"

void FastPWM::init(void) {
  this->off();//HEATER_PORT &= ~HEATER_BITMASK;  // Switch-off the power
  HEATER_DDR |= HEATER_BITMASK; //pinMode(10, OUTPUT);  // Use D10 pin for heating the IRON

  tmr1_count = 0;
  iron_off = false;  // The volatile global variable
  noInterrupts();
  TCNT1 = 0;
  TCCR1B = _BV(WGM13);  // Set mode as phase and frequency correct pwm, stop the timer
  TCCR1A = 0;
  ICR1 = 256;
  TCCR1B = _BV(WGM13) | _BV(CS10);  // Top value = ICR1, prescale = 1; 31250 Hz
  TCCR1A |= _BV(COM1B1);            // XOR D10 on OC1B, detached from D09
  OCR1B = 0;                        // Switch-off the signal on pin D10;
  TIMSK1 = _BV(TOIE1);              // Enable overflow interrupts @31250 Hz
  interrupts();
}


void IRON::init(void) {
  pinMode(sPIN, INPUT);
  pinMode(aPIN, INPUT);
  pinMode(tPIN, INPUT);
  fastPWM.init();  // Initialization for 31.5 kHz PWM on D10 pin
  mode = POWER_OFF;
  fix_power = 0;
  applied_power = 0;
  disconnected = true;  // Do not read the ambient temperature when the IRON is connected
  check_iron_ms = 0;
  resetPID(h_temp.read());
  h_counter = h_max_counter;
  h_power.init();
  h_temp.init();
  current.length(emp_k);
  amb_int.length(4);
}

void IRON::setTemp(uint16_t t) {
  if (mode == POWER_ON)
    resetPID(h_temp.read());
  temp_set = t;
  uint16_t ta = h_temp.average();
  chill = (ta > t + 5);  // The IRON must be cooled
  temp_low = 0;          // disable low power mode
}

void IRON::lowPowerMode(uint16_t t) {
  if ((mode == POWER_ON && t < temp_set) || t == 0)
    temp_low = t;  // Activate low power mode
  if (t == 0)      //IF GET OUT OF LOW POWER MODE RESET PID
    resetPID(h_temp.read());
}

uint8_t IRON::getAvgPower(void) {
  uint16_t p = h_power.average();
  return p & 0xff;
}

uint8_t IRON::appliedPower(void) {
  uint8_t p = getAvgPower();
  return map(p, 0, max_power, 0, 100);
}

void IRON::switchPower(bool on) {
  if (!on) {
    fastPWM.off();
    fix_power = 0;
    LED_PORT &= ~LED_BITMASK;
    //disp->noBacklight();
    if (mode != POWER_OFF)
      mode = POWER_COOLING;
    return;
  }
  //disp->backlight();
  h_power.init();
  mode = POWER_ON;
  lowPowerMode(0);  //GET OUT OF LOW POWER MODE AND RESET PID resetting is handled in lowPowerMode() Artins fix
}

bool IRON::isOn(void) {
  return (mode == POWER_ON || mode == POWER_FIXED);
}

bool IRON::checkIron(void) {
  if (millis() < check_iron_ms)
    return disconnected;

  check_iron_ms = millis() + check_period;
  uint16_t curr = 0;
  if (applied_power == 0) {            // The IRON is switched-off
    fastPWM.duty(50);                 // Quarter of maximum power
    for (uint8_t i = 0; i < 5; ++i) {  // Make sure we check the current in active phase of PWM signal
      delayMicroseconds(31);
      uint16_t c = analogRead(cPIN);  // Check the current through the IRON
      if (c > curr) curr = c;         // The maximum value
    }
    fastPWM.off();
    if (curr > min_curr * 2)       // Do not put big values in to the history
      curr = min_curr * 2;         // This is enough to ensure the IRON is connected
    curr = current.average(curr);  // Calculate exponential average value of the current
  } else {
    curr = analogRead(cPIN);
  }
  if (mode == POWER_OFF || mode == POWER_COOLING) {  // If the soldering IRON is set to be switched off
    fastPWM.off();                                   // Surely power off the IRON
  }
  disconnected = (curr < min_curr);
#ifdef DEBUGEEE
  // Serial.println(curr);
#endif
  return disconnected;
}

// This routine is used to keep the IRON temperature near required value and is activated by the Timer1
void IRON::keepTemp(void) {
  uint16_t ambient = analogRead(aPIN);  // Update ambient temperature
  amb_int.update(ambient);
  uint16_t t = analogRead(sPIN);       // Read the IRON temperature
  volatile uint16_t t_set = temp_set;  // The preset temperature depends on usual/low power mode
  if (temp_low) t_set = temp_low;

  if ((t >= temp_max + 20) || (t > (t_set + 100))) {  // Prevent global over heating
    if (mode == POWER_ON) chill = true;               // Turn off the power in main working mode only;
  }
  if (t < temp_max) {  // Do not save to the history readings when the IRON is disconnected
    if (--h_counter < 0) {
      h_temp.update(t);
      h_counter = h_max_counter;
    }
  }

  int32_t p = 0;
  switch (mode) {
    case POWER_OFF:
      break;
    case POWER_COOLING:
      if (h_temp.average() < iron_cold) {
        mode = POWER_OFF;
      }

      break;
    case POWER_ON:
      if (chill) {
        if (t < (t_set - 2)) {
          chill = false;
          resetPID(h_temp.read());
        } else {
          break;
        }
      }
      p = PID::reqPower(t_set, t);
      p = constrain(p, 0, max_power);
      break;
    case POWER_FIXED:
      p = fix_power;
      break;
    default:
      break;
  }
  applied_power = p & 0xff;
  if (h_counter == 1) {
    h_power.update(applied_power);
  }
  fastPWM.duty(applied_power);
}

void IRON::fixPower(uint8_t Power) {
  if (Power == 0) {  // To switch off the IRON, set the Power to 0
    fix_power = 0;
    fastPWM.off();
    mode = POWER_COOLING;
    return;
  }

  if (Power > max_fixed_power)
    Power = max_fixed_power;
  fix_power = Power;
  mode = POWER_FIXED;
}

/*
   Return ambient temperature in Celsius
   Caches previous result to skip expensive calculations
*/



int16_t IRON::ambientTemp(void) {
  static const uint16_t add_resistor = NTC_PULLUP;                           // The additional resistor value (10koHm)
  static const float normal_temp[2] = { NTC_RESISTANCE, NTC_AMBIENT_TEMP };  // nominal resistance and the nominal temperature
  static const uint16_t beta = NTC_BETA;                                     // The beta coefficient of the thermistor (usually 3000-4000)
  static int32_t average = -1;                                               // Previous value of ambient temperature (readings on aPIN)
  static int cached_ambient = ambient_tempC;                                 // Previous value of the temperature

  uint16_t a_temp = amb_int.read();  // Average value of ambient temperature
  if (abs(a_temp - average) < 20)
    return cached_ambient;

  average = a_temp;
  if (average < 975) {  // prevent division by zero
    // convert the value to resistance
    float resistance = 1023.0 / (float)average - 1.0;
    resistance = (float)add_resistor / resistance;
    float steinhart = resistance / normal_temp[0];  // (R/Ro)
    steinhart = log(steinhart);                     // ln(R/Ro)
    steinhart /= beta;                              // 1/B * ln(R/Ro)
    steinhart += 1.0 / (normal_temp[1] + 273.15);   // + (1/To)
    steinhart = 1.0 / steinhart;                    // Invert
    steinhart -= 273.15;                            // convert to Celsius
    cached_ambient = round(steinhart);
  } else {  // about -30 *C, perhaps, the IRON is disconnected
    cached_ambient = ambient_tempC;
  }
  return cached_ambient;
}

void IRON::adjust(uint16_t t) {
  if (t > temp_max) t = temp_max;  // Do not allow over heating
  temp_set = t;
}

//IF IRON IS ON STAND TILT_TOGLE is set, returns if recently changed
void IRON::checkSWStatus(void) {
  if (millis() > check_tilt_ms) {
    check_tilt_ms = millis() + 30;
    if (!disconnected) {  // Current through the IRON is registered
      uint16_t avg = tilt.read();
      if (300 < avg && avg < 700) {  // Middle state
        avg = tilt.average(analogRead(tPIN));
        if (avg < 300 || avg > 700) {  // Toggle state
          tilt_toggle = true;
        }
      } else {
        tilt.update(analogRead(tPIN));
      }
    }
  }
}

bool IRON::isIronTiltSwitch(bool reed) {
  bool ret = tilt_toggle;
  tilt_toggle = false;
  if (reed) {
    return tilt.read() > 600;
  }
  return ret;
}
