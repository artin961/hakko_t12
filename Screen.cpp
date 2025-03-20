#include "Screen.h"

void mainSCREEN::init(void) {
  pIron->switchPower(false);
  pD->noBacklight();
  uint16_t temp_set = pIron->presetTemp();
  int16_t ambient = pIron->ambientTemp();
  temp_set = pCfg->tempToHuman(temp_set, ambient);  // The preset temperature in the human readable units
  if (pCfg->isCelsius())
    pEnc->reset(temp_set, temp_minC, temp_maxC, 1, 5);
  else
    pEnc->reset(temp_set, temp_minF, temp_maxF, 1, 5);
  used = !pIron->isCold();
  cool_notified = !used;
  if (used) {  // the IRON was used, we should save new data in EEPROM
    pCfg->savePresetTempHuman(temp_set);
  }
  clear_used_ms = 0;
  forceRedraw();
  pD->clear();
  pD->msgOff();
  show_tip = false;
  change_display = millis() + show_temp;
}

void mainSCREEN::rotaryValue(int16_t value) {
  // no as we have other values - updated(); //Mark screen just updated
  int16_t ambient = pIron->ambientTemp();
  uint16_t temp = pCfg->humanToTemp(value, ambient);
  pIron->setTemp(temp);
  pD->tSet(value, pCfg->isCelsius());
  change_display = millis() + show_temp;
  show_tip = false;
}

SCREEN* mainSCREEN::render(uint32_t ms) {
  SCREEN* nxt = this;
  // If no Iron connected swith screen
  if (no_iron && pIron->checkIron()) {  // Check that the IRON is connected
    nxt = no_iron;
  }

  if (clear_used_ms && (ms > clear_used_ms)) {
    clear_used_ms = 0;
    used = false;
  }

  if (ms > change_display) {
    show_tip = !show_tip;
    change_display = ms + 2000;
    if (!show_tip) change_display += show_temp;
  }

  int16_t ambient = pIron->ambientTemp();
  if (show_tip) {
    pD->tip(pCfg->tipName(), true);
  } else {
    uint16_t temp_set = pIron->presetTemp();
    temp_set = pCfg->tempToHuman(temp_set, ambient);  // The preset temperature in the human readable units
    pD->tSet(temp_set, pCfg->isCelsius());
  }
  pD->msgOff();

  uint16_t temp = pIron->tempAverage();
  uint16_t tempH = pCfg->tempToHuman(temp, ambient);
  if (pIron->isCold()) {
    if (used)
      pD->msgCold();
    else
      pD->tCurr(tempH);
    if (used && !cool_notified) {
      pBz->lowBeep();
      cool_notified = true;
      clear_used_ms = ms + cool_notify_period;
    }
  } else {
    pD->tCurr(tempH);
  }
  return nxt;
}


void tipSCREEN::init(void) {
  pIron->switchPower(false);
  old_tip = pCfg->tipIndex();
  pEnc->reset(old_tip, 0, pCfg->tipsLoaded(), 1, 1, true);  // Select the tip by the rotary encoder
  forceRedraw();
  pD->clear();
  pD->msgSelectTip();
}

void tipSCREEN::rotaryValue(int16_t value) {
  uint8_t index = pCfg->nextTip(old_tip, value > old_tip);
  uint16_t temp = pIron->presetTemp();  // Preset temperature in internal units
  int16_t ambient = pIron->ambientTemp();
  temp = pCfg->tempToHuman(temp, ambient);  // The temperature in human readable units (Celsius o Fahrenheit)
  index = pCfg->selectTip(index);
  old_tip = index;
  pEnc->write(index);
  temp = pCfg->humanToTemp(temp, ambient);  // Translate previously set temperature in human readable units into internal value
  pIron->setTemp(temp);                     // Install previously set temperature into the IRON by new tip calibration
  forceRedraw();
}

SCREEN* tipSCREEN::render(uint32_t ms) {
  SCREEN* nxt = this;
  if (no_iron && !pIron->checkIron()) {  // Check that the IRON is disconnected
    nxt = no_iron;
    pIron->initTempHistory();  // The new tip is connected, reset the temp history
  }

  pD->tip(pCfg->tipName(), false);
  pD->mark('*', !pCfg->isCalibrated());
  return nxt;
}

void actSCREEN::init(void) {
  pIron->switchPower(false);
  char* n = pCfg->tipName();
  int8_t global_index = pCfg->index(n);  // Find current tip in the global tip array by the name
  if (global_index < 0) global_index = 0;
  pEnc->reset(global_index, 0, pCfg->tipsLoaded(), 1, 1, true);  // Select the tip by the rotary encoder
  pD->clear();
  pD->msgActivateTip();
  pD->tip(pCfg->tipName(), false);
}

void actSCREEN::rotaryValue(int16_t value) {
  forceRedraw();
}

SCREEN* actSCREEN::menu(void) {
  uint8_t tip = pEnc->read();
  bool active = pCfg->toggleTipActivation(tip);
  forceRedraw();
  return this;
}

SCREEN* actSCREEN::render(uint32_t ms) {
  uint8_t tip = pEnc->read();
  bool active = pCfg->isTipActive(tip);
  char tip_name[tip_name_sz + 1];
  pCfg->name(tip_name, tip);
  tip_name[tip_name_sz] = '\0';
  pD->clear();
  pD->tip(tip_name, false);
  pD->mark('x', active);
  return this;
}

void workSCREEN::init(void) {
  pD->backlight();
  uint16_t temp_set = pIron->presetTemp();
  int16_t ambient = pIron->ambientTemp();
  bool is_celsius = pCfg->isCelsius();
  tempH = pCfg->tempToHuman(temp_set, ambient);
  if (is_celsius)
    pEnc->reset(tempH, temp_minC, temp_maxC, 1, 5);
  else
    pEnc->reset(tempH, temp_minF, temp_maxF, 1, 5);
  pIron->switchPower(true);
  
  ready = false;
  lowpower_mode = false;
  lowpower_time = 0;
  time_to_return = 0;
  pD->clear();
  pD->tSet(tempH, is_celsius);
  pD->msgOn();
  uint16_t to = pCfg->getOffTimeout() * 60;
  this->setSCRtimeout(to);
  auto_off_notified = 0;
  forceRedraw();
}

void workSCREEN::rotaryValue(int16_t value) {  // Setup new preset temperature by rotating the encoder
  tempH = value;
  ready = false;
  lowpower_mode = false;
  pD->msgOn();
  int16_t ambient = pIron->ambientTemp();
  uint16_t temp = pCfg->humanToTemp(value, ambient);  // Translate human readable temperature into internal value
  pIron->setTemp(temp);
  pD->tSet(value, pCfg->isCelsius());
  resetTimeout();
}

SCREEN* workSCREEN::render(uint32_t ms) {
  SCREEN* nxt = this;
  if (no_iron && pIron->checkIron()) {  // Check that the IRON is connected
    nxt = no_iron;
  }
  if (ready)
    LED_PORT |= LED_BITMASK;
  else
    LED_PORT ^= LED_BITMASK;

  int16_t temp = pIron->tempAverage();
  int16_t temp_set = pIron->presetTemp();
  int16_t ambient = pIron->ambientTemp();
  int tempH = pCfg->tempToHuman(temp, ambient);
  pD->tCurr(tempH);
  uint8_t p = pIron->appliedPower();
  pD->percent(p);

  // uint16_t td = pIron->tempDispersion();
  //uint16_t pd = pIron->powerDispersion();
  int ap = pIron->getAvgPower();
  uint16_t low_temp = pCfg->lowTemp();  // 'Standby temperature' setup in the main menu
  if ((abs(temp_set - temp) < 3) && (pIron->tempDispersion() <= 150) && (ap > 0)) {
    if (!ready) {
      pBz->shortBeep();
      ready = true;
      pD->msgReady();
      if (low_temp)
        lowpower_time = ms + (uint32_t)pCfg->lowTimeout() * 1000;
      update_screen = ms + (update_period << 2);
      return this;
    }
  }

  bool tilt_active = false;
  if (low_temp) {
    tilt_active = pIron->isIronTiltSwitch(pCfg->isReedType());
    if (tilt_active) {
      resetTimeout();
    }
  }

  // If the automatic power-off feature is enabled, check the IRON status
  if (low_temp && ready && pCfg->getOffTimeout()) {  // The IRON has reaches the preset temperature
    hwTimeout(low_temp, tilt_active);                // Use hardware tilt switch to turn low power mode
  }

  if (!lowpower_mode && pCfg->isAmbientSensor())
    adjustPresetTemp();

  uint32_t to = (time_to_return - ms) / 1000;
  if (ready) {
    if (scr_timeout > 0 && (to < 100)) {
      pD->timeToOff(to);
      if (!auto_off_notified) {
        pBz->shortBeep();
        auto_off_notified = ms;
      }
    } else if (lowpower_mode) {
      pD->msgStandby();
    } else if (wasRecentlyReset()) {
      pD->msgReady();
    } else {
      pD->msgReady();
    }
  } else {
    pD->msgOn();
    resetTimeout();
  }
  return nxt;
}

SCREEN* workSCREEN::returnToMain(void) {
  if (main && scr_timeout && (millis() >= time_to_return)) {
    scr_timeout = 0;
    pBz->doubleBeep();
    return main;
  }
  return this;
}

void workSCREEN::adjustPresetTemp(void) {
  uint16_t presetTemp = pIron->presetTemp();  // Preset temperature (internal units)
  int16_t ambient = pIron->ambientTemp();
  uint16_t temp = pCfg->humanToTemp(tempH, ambient);  // Expected temperature of IRON in internal units
  if (temp != presetTemp) {                           // The ambient temperature have changed, we need to adjust preset temperature
    pIron->adjust(temp);
  }
}

void workSCREEN::hwTimeout(uint16_t low_temp, bool tilt_active) {
  uint32_t now_ms = millis();
  if (tilt_active) {                                               // If the IRON is used, Reset standby time
    lowpower_time = now_ms + (uint32_t)pCfg->lowTimeout() * 1000;  // Convert timeout to milliseconds
    if (lowpower_mode) {
      ///artin// If the IRON is in low power mode, return to main working mode
      pIron->lowPowerMode(0);
      lowpower_time = 0;
      lowpower_mode = false;
      ready = false;
      pD->msgOn();
      resetTimeout();  // Activate automatic power-off
    }
  } else if (!lowpower_mode) {        //if iron is not used
    if (lowpower_time) {              //and we are not in low power mode
      if (now_ms >= lowpower_time) {  //and it is time to go to low power mode
        int16_t ambient = pIron->ambientTemp();
        uint16_t temp_low = pCfg->lowTemp();
        uint16_t temp = pCfg->humanToTemp(temp_low, ambient);
        pIron->lowPowerMode(temp);
        auto_off_notified = false;  //reset timeout for auto power off
        lowpower_mode = true;
        resetTimeout();  // Activate automatic power-off
        return;
      }
    } else {
      lowpower_time = now_ms + (uint32_t)pCfg->lowTimeout() * 1000;
    }
  }
}




void powerSCREEN::init(void) {
  uint8_t p = pIron->getAvgPower();
  uint8_t max_power = pIron->getMaxFixedPower();
  pEnc->reset(p, 0, max_power, 1);
  on = true;  // Do start heating immediately
  pIron->switchPower(false);
  pIron->fixPower(p);
  pD->clear();
  pD->pSet(p);
}

SCREEN* powerSCREEN::render(uint32_t ms) {

  SCREEN* nxt = this;
  if (no_iron && pIron->checkIron()) {  // Check that the IRON is connected
    nxt = no_iron;
  }
  uint16_t temp = pIron->tempAverage();
  int16_t ambient = pIron->ambientTemp();
  temp = pCfg->tempToHuman(temp, ambient);
  pD->tCurr(temp);
  return nxt;
}

void powerSCREEN::rotaryValue(int16_t value) {
  pD->pSet(value);
  pIron->fixPower(value);
  on = true;
  updated(); //Mark screen just updated
}

SCREEN* powerSCREEN::menu(void) {
  on = !on;
  if (on) {
    uint16_t pos = pEnc->read();
    pIron->fixPower(pos);
    pD->clear();
    pD->pSet(pos);
  } else {
    pIron->fixPower(0);
    pD->clear();
    pD->pSet(0);
  }
  forceRedraw();
  return this;
}

SCREEN* powerSCREEN::menu_long(void) {
  pIron->fixPower(0);
  if (nextL) {
    pIron->switchPower(true);
    return nextL;
  }
  return this;
}






void configSCREEN::init(void) {
  pD->backlight();
  mode = 0;
  // 0 - C/F, 1 - buzzer, 2 - switch type, 3 - ambient, 4 - standby temp, 5 - standby time,
  // 6 - off-timeout, 7 - tip calibrate, 8 - atcivate tip, 9 - tune, 10 - save, 11 - cancel
  pEnc->reset(mode, 0, 11, 1, 0, true);
  tune = false;
  changed = false;
  cels = pCfg->isCelsius();
  buzzer = pCfg->isBuzzer();
  reed = pCfg->isReedType();
  ambient = pCfg->isAmbientSensor();
  off_timeout = pCfg->getOffTimeout();
  low_temp = pCfg->lowTemp();
  low_timeout = pCfg->lowTimeout();
  pD->clear();
  pD->setupMode(0, false, 0);
  this->setSCRtimeout(30);
}

SCREEN* configSCREEN::render(uint32_t ms) {
  switch (mode) {
    case 0:  // C/F
      pD->setupMode(mode, tune, cels, cels);
      break;
    case 1:  // buzzer
      pD->setupMode(mode, tune, buzzer, cels);
      break;
    case 2:  // switch type
      pD->setupMode(mode, tune, reed, cels);
      break;
    case 3:  // ambient temperature sensor
      pD->setupMode(mode, tune, ambient, cels);
      break;
    case 4:  // standby temp
      pD->setupMode(mode, tune, low_temp, cels);
      break;
    case 5:  // standby timeout
      pD->setupMode(mode, tune, low_timeout, cels);
      break;
    case 6:  // off-timeout
      pD->setupMode(mode, tune, off_timeout, cels);
      break;
    default:
      pD->setupMode(mode, false, 0, cels);
      break;
  }
  return this;
}

void configSCREEN::rotaryValue(int16_t value) {
  if (tune) {  // tune the temperature units
    changed = true;
    switch (mode) {
      case 0:  // Buzzer
        cels = value;
        break;
      case 1:  // Buzzer
        buzzer = value;
        break;
      case 2:  // Switch type
        reed = value;
        break;
      case 3:  // ambient temperature sensor
        ambient = value;
        break;
      case 4:  // standby temperature
        if (value > 0)
          value += 100;
        low_temp = value;
        break;
      case 5:  // Standby Time
        low_timeout = value;
        break;
      case 6:                       // tuning the switch-off timeout
        if (value > 0) value += 2;  // The minimum timeout is 3 minutes
        off_timeout = value;
        break;
      default:
        break;
    }
  } else {
    mode = value;
  }
  forceRedraw();
}

SCREEN* configSCREEN::menu(void) {
  if (tune) {
    tune = false;
    pEnc->reset(mode, 0, 11, 1, 0, true);  // The value has been tuned, return to the menu list mode
  } else {
    switch (mode) {
      case 0:  // C/F. In-line editing
        //cels = !cels;
        //changed = true;
        //forceRedraw();
        // return this;
        pEnc->reset(cels, 0, 1, 1, 0, true);
        break;
      case 1:  // Buzzer
        pEnc->reset(buzzer, 0, 1, 1, 0, true);
        break;
      case 2:  // Switch type
        pEnc->reset(reed, 0, 1, 1, 0, true);
        break;
      case 3:  // ambient temperature sensor
        pEnc->reset(ambient, 0, 1, 1, 0, true);
        break;
      case 4:  // standby temperature
        {
          uint16_t v = low_temp;
          if (v > 0) v -= 100;
          pEnc->reset(v, 0, 120, 1, 5, false);
        }
        break;
      case 5:  // standby timeout
        pEnc->reset(low_timeout, 3, 255, 1, 5, false);
        break;
      case 6:  // off-timeout
        {
          int v = off_timeout;
          if (v > 0) v -= 2;
          pEnc->reset(v, 0, 28, 1, 5, false);
        }
        break;
      case 7:
        if (calib) return calib;
        break;
      case 8:  // Activate tip
        if (activate) return activate;
        break;
      case 9:  // Tune potentiometer
        if (next) return next;
        break;
      case 10:  // Save configuration data
        menu_long();
      case 11:  // Return to the main menu
        if (main) return main;
        return this;
    }
    tune = true;
  }
  forceRedraw();
  return this;
}

SCREEN* configSCREEN::menu_long(void) {
  if (nextL) {
    if (changed) {
      pCfg->saveConfig(off_timeout, cels, buzzer, ambient, low_temp, low_timeout, reed);
      pBz->activate(buzzer);
    }
    return nextL;
  }
  return this;
}




void calibSCREEN::init(void) {
  pIron->switchPower(false);
  cels = true;
  // Prepare to enter real temperature
  uint16_t min_t = 50;
  uint16_t max_t = 600;
  if (!pCfg->isCelsius()) {
    min_t = 122;
    max_t = 1111;
    cels = false;
  }
  pEnc->reset(0, min_t, max_t, 1, 5, false);
  tip_temp_max = temp_max / 2;
  for (uint8_t i = 0; i < MCALIB_POINTS; ++i) {
    calib_temp[0][i] = 0;                                                           // Real temperature. 0 means not entered yet
    calib_temp[1][i] = map(i, 0, MCALIB_POINTS - 1, start_int_temp, tip_temp_max);  // Internal temperature
  }
  ready = false;
  ref_temp_index = 0;
  pD->clear();
  pD->tRef(ref_temp_index);
  preset_temp = pIron->presetTemp();  // Save the preset temperature in human readable units
  int16_t ambient = pIron->ambientTemp();
  preset_temp = pCfg->tempToHuman(preset_temp, ambient);
  uint16_t temp = calib_temp[1][ref_temp_index];
  pIron->setTemp(temp);
  pIron->switchPower(true);
  pD->msgOn();
  forceRedraw();
}

SCREEN* calibSCREEN::render(uint32_t ms) {
  int16_t temp = pIron->tempAverage();  // Actual IRON temperature
  int16_t temp_set = pIron->presetTemp();
  int16_t ambient = pIron->ambientTemp();
  uint16_t tempH = pCfg->tempToHuman(temp, ambient);
  if (ready) {
    pD->tSet(tempH, cels);
  } else {  // Show the current Iron temperature
    pD->tCurr(tempH);
  }
  uint8_t p = pIron->appliedPower();
  if (!pIron->isOn()) p = 0;
  pD->percent(p);
  if (!ready && abs(temp_set - temp) < 4 && pIron->tempDispersion() <= 20) {
    pBz->shortBeep();
    pD->msgReady();
    ready = true;
    pEnc->write(tempH);
  }
  if (ready && !pIron->isOn()) {  // The IRON was switched off by error
    pD->msgOff();
    ready = false;
  }
  return this;
}

void calibSCREEN::rotaryValue(int16_t value) {  // The Encoder rotated
  updated(); //Mark screen just updated
  if (ready) {  // change the real value for the temperature
    pD->tCurr(value);
  }
}

SCREEN* calibSCREEN::menu(void) {  // Rotary encoder pressed
  if (ready) {                     // The real temperature has been entered
    uint16_t r_temp = pEnc->read();
    uint16_t temp = pIron->tempAverage();  // The temperature of the IRON in internal units
    pIron->switchPower(false);
    pD->msgOff();
    if (!cels)  // Always save the human readable temperature in Celsius
      r_temp = map(r_temp, temp_minF, temp_maxF, temp_minC, temp_maxC);
    calib_temp[0][ref_temp_index] = r_temp;
    calib_temp[1][ref_temp_index] = temp;
    if (r_temp < temp_maxC - 20) {
      updateReference(ref_temp_index);  // Update reference temperature points
      ++ref_temp_index;
      // Try to update the current tip calibration
      uint16_t tip[3];
      if (calibrationOLS(tip, 150, 600)) {
        pCfg->applyCalibration(tip);
      }
    }
    if ((r_temp >= temp_maxC - 20) || ref_temp_index >= MCALIB_POINTS) {
      return menu_long();  // Finish calibration
    } else {               // Continue calibration
      uint16_t temp = calib_temp[1][ref_temp_index];
      pIron->setTemp(temp);
      pIron->switchPower(true);
      pD->msgOn();
    }
  } else {  // Toggle the power
    if (pIron->isOn()) {
      pIron->switchPower(false);
      pD->msgOff();
    } else {
      pIron->switchPower(true);
      pD->msgOn();
    }
  }
  pD->tRef(ref_temp_index);
  ready = false;
  forceRedraw();
  return this;
}

SCREEN* calibSCREEN::menu_long(void) {  // Save new tip calibration data
  pIron->switchPower(false);
  uint16_t tip[3];
  if (calibrationOLS(tip, 150, temp_maxC)) {
    uint8_t near_index = closestIndex(temp_tip[2]);
    tip[2] = map(temp_tip[2], temp_tip[1], calib_temp[0][near_index],
                 tip[1], calib_temp[1][near_index]);
    if (tip[2] > temp_max) tip[2] = temp_max;

    pCfg->applyCalibration(tip);
    pCfg->saveCalibrationData(tip, ambient_tempC);
  }
  pCfg->savePresetTempHuman(preset_temp);
  int16_t ambient = pIron->ambientTemp();
  uint16_t temp = pCfg->humanToTemp(preset_temp, ambient);
  pIron->setTemp(temp);
  if (nextL) return nextL;
  return this;
}

/*
   Calculate tip calibration parameter using linear approximation by Ordinary Least Squares method
   Y = a * X + b, where
   Y - internal temperature, X - real temperature. a and b are double coefficients
   a = (N * sum(Xi*Yi) - sum(Xi) * sum(Yi)) / ( N * sum(Xi^2) - (sum(Xi))^2)
   b = 1/N * (sum(Yi) - a * sum(Xi))
*/
bool calibSCREEN::calibrationOLS(uint16_t* tip, uint16_t min_temp, uint16_t max_temp) {
  long sum_XY = 0;  // sum(Xi * Yi)
  long sum_X = 0;   // sum(Xi)
  long sum_Y = 0;   // sum(Yi)
  long sum_X2 = 0;  // sum(Xi^2)
  long N = 0;

  for (uint8_t i = 0; i < MCALIB_POINTS; ++i) {
    uint16_t X = calib_temp[0][i];
    uint16_t Y = calib_temp[1][i];
    if (X >= min_temp && X <= max_temp) {
      sum_XY += X * Y;
      sum_X += X;
      sum_Y += Y;
      sum_X2 += X * X;
      ++N;
    }
  }

  if (N <= 2)  // Not enough real temperatures have been entered
    return false;

  double a = (double)N * (double)sum_XY - (double)sum_X * (double)sum_Y;
  a /= (double)N * (double)sum_X2 - (double)sum_X * (double)sum_X;
  double b = (double)sum_Y - a * (double)sum_X;
  b /= (double)N;

  for (uint8_t i = 0; i < 3; ++i) {
    double temp = a * (double)temp_tip[i] + b;
    tip[i] = round(temp);
  }
  if (tip[2] > temp_max) tip[2] = temp_max;
  return true;
}

// Find the index of the reference point with the closest temperature
uint8_t calibSCREEN::closestIndex(uint16_t temp) {
  uint16_t diff = 1000;
  uint8_t index = MCALIB_POINTS;
  for (uint8_t i = 0; i < MCALIB_POINTS; ++i) {
    uint16_t X = calib_temp[0][i];
    if (X > 0 && abs(X - temp) < diff) {
      diff = abs(X - temp);
      index = i;
    }
  }
  return index;
}

void calibSCREEN::updateReference(uint8_t indx) {  // Update reference points
  uint16_t expected_temp = map(indx, 0, MCALIB_POINTS, temp_minC, temp_maxC);
  uint16_t r_temp = calib_temp[0][indx];
  if (indx < 5 && r_temp > (expected_temp + expected_temp / 4)) {  // The real temperature is too high
    tip_temp_max -= tip_temp_max >> 2;                             // tip_temp_max *= 0.75;
    if (tip_temp_max < temp_max / 4)
      tip_temp_max = temp_max / 4;                            // Limit minimum possible value of the highest temperature
  } else if (r_temp > (expected_temp + expected_temp / 8)) {  // The real temperature is biger than expected
    tip_temp_max += tip_temp_max >> 3;                        // tip_temp_max *= 1.125;
    if (tip_temp_max > temp_max)
      tip_temp_max = temp_max;
  } else if (indx < 5 && r_temp < (expected_temp - expected_temp / 4)) {  // The real temperature is too low
    tip_temp_max += tip_temp_max >> 2;                                    // tip_temp_max *= 1.25;
    if (tip_temp_max > temp_max)
      tip_temp_max = temp_max;
  } else if (r_temp < (expected_temp - expected_temp / 8)) {  // The real temperature is lower than expected
    tip_temp_max += tip_temp_max >> 3;                        // tip_temp_max *= 1.125;
    if (tip_temp_max > temp_max)
      tip_temp_max = temp_max;
  } else {
    return;
  }
  // rebuild the array of the reference temperatures
  for (uint8_t i = indx + 1; i < MCALIB_POINTS; ++i) {
    calib_temp[1][i] = map(i, 0, MCALIB_POINTS - 1, start_int_temp, tip_temp_max);
  }
}




void tuneSCREEN::init(void) {
  pIron->switchPower(false);
  max_power = pIron->getMaxFixedPower();
  pEnc->reset(15, 0, max_power, 1, 5);  // Rotate the encoder to change the power supplied
  arm_beep = false;
  pD->clear();
  pD->msgTune();
  forceRedraw();
}

void tuneSCREEN::rotaryValue(int16_t value) {
  pIron->fixPower(value);
  forceRedraw();
}

SCREEN* tuneSCREEN::render(uint32_t ms) {
  int16_t temp = pIron->tempAverage();
  pD->tCurr(temp);
  uint8_t power = pEnc->read();  // applied power
  if (!pIron->isOn())
    power = 0;
  else
    power = map(power, 0, max_power, 0, 100);
  pD->percent(power);
  if (arm_beep && (pIron->tempDispersion() < 5)) {
    pBz->shortBeep();
    arm_beep = false;
  }
  return this;
}

SCREEN* tuneSCREEN::menu(void) {  // The rotary button pressed
  if (pIron->isOn()) {
    pIron->fixPower(0);
  } else {
    uint8_t power = pEnc->read();  // applied power
    pIron->fixPower(power);
  }
  return this;
}

SCREEN* tuneSCREEN::menu_long(void) {
  pIron->fixPower(0);  // switch off the power
  if (next) return next;
  return this;
}




void pidSCREEN::init(void) {
  temp_set = pIron->presetTemp();
  mode = 0;                          // select the element from the list
  pEnc->reset(1, 1, 4, 1, 1, true);  // 1 - Kp, 2 - Ki, 3 - Kd, 4 - temp
  showCfgInfo();
#ifdef DEBUGEEE
  Serial.println("");
#endif
}
/*
  void pidSCREEN::rotaryValue(int16_t value) {
  if (mode == 0) {  // No limit is selected, list the menu
    showCfgInfo();
  #ifdef DEBUGEEE
    switch (value) {
      case 1:
        Serial.println("Kp");
        break;
      case 2:
        Serial.println("Ki");
        break;
      case 4:
        Serial.println(F("Temp"));
        break;
      case 3:
      default:
        Serial.println("Kd");
        break;
    }
  #endif
  } else {
  #ifdef DEBUGEEE
    switch (mode) {
      case 1:
        Serial.print(F("Kp = "));
        pIron->changePID(mode, value);
        break;
      case 2:
        Serial.print(F("Ki = "));
        pIron->changePID(mode, value);
        break;
      case 4:
        Serial.print(F("Temp = "));
        temp_set = value;
        pIron->setTemp(value);
        break;
      case 3:
      default:
        Serial.print(F("Kd = "));
        pIron->changePID(mode, value);
        break;
    }
    Serial.println(value);
  #endif
  }
  }

  SCREEN* pidSCREEN::render(uint32_t ms) {
  //if (millis() < update_screen) return this;
  //update_screen = millis() + period;
  if (pIron->isOn()) {
  #ifdef DEBUGEEE
    char buff[60];
    int temp = pIron->currTemp();
    uint8_t pwr = pIron->getAvgPower();
    uint16_t td = pIron->tempDispersion();
    uint16_t pd = pIron->powerDispersion();
    sprintf(buff, "%3d: power = %3d, td = %3d, pd = %3d --- ", temp_set - temp, pwr, td, pd);
    Serial.println(buff);
  #endif
  }
  return this;
  }

  SCREEN* pidSCREEN::menu(void) {  // The rotary button pressed
  if (mode == 0) {               // select upper or lower temperature limit
    mode = pEnc->read();
    if (mode != 4) {
      int k = pIron->changePID(mode, -1);
      pEnc->reset(k, 0, 10000, 1, 10);
    } else {
      pEnc->reset(temp_set, 0, 970, 1, 5);
    }
  } else {  // upper or lower temperature limit just setup
    mode = 0;
    pEnc->reset(1, 1, 4, 1, 1, true);  // 1 - Kp, 2 - Ki, 3 - Kd, 4 - temp
  }
  return this;
  }

  SCREEN* pidSCREEN::menu_long(void) {
  bool on = pIron->isOn();
  pIron->switchPower(!on);
  return this;
  }

  void pidSCREEN::showCfgInfo(void) {
  for (uint8_t i = 1; i < 4; ++i) {
    int k = pIron->changePID(i, -1);
  }
  }
*/
