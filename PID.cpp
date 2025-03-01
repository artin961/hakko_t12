#include "PID.h"

void PID::resetPID(int temp) {
  temp_h0 = 0;
  power = 0;
  i_summ = 0;
  pid_iterate = false;
  if ((temp > 0) && (temp < 1000))
    temp_h1 = temp;
  else
    temp_h1 = 0;
}
int16_t PID::changePID(uint8_t p, int k) {
  switch (p) {
    case 1:
      if (k >= 0) Kp = k;
      return Kp;
    case 2:
      if (k >= 0) Ki = k;
      return Ki;
    case 3:
      if (k >= 0) Kd = k;
      return Kd;
    default:
      break;
  }
  return 0;
}

int32_t PID::reqPower(int temp_set, int temp_curr) {
  if (temp_h0 == 0) {
    // When the temperature is near the preset one, reset the PID and prepare iterative formula
    if ((temp_set - temp_curr) < 30) {
      if (!pid_iterate) {
        pid_iterate = true;
        power = 0;
        i_summ = 0;
      }
    }
    i_summ += temp_set - temp_curr;  // first, use the direct formula, not the iterate process
    power = Kp * (temp_set - temp_curr) + Ki * i_summ;
    // If the temperature is near, prepare the PID iteration process
  } else {
    int32_t kp = Kp * (temp_h1 - temp_curr);
    int32_t ki = Ki * (temp_set - temp_curr);
    int32_t kd = Kd * (temp_h0 + temp_curr - 2 * temp_h1);
    int32_t delta_p = kp + ki + kd;
    power += delta_p;  // power kept multiplied by denominator!
  }
  if (pid_iterate) temp_h0 = temp_h1;
  temp_h1 = temp_curr;
  int32_t pwr = power + (1 << (denominator_p - 1));  // prepare the power to delete by denominator, round the result
  pwr >>= denominator_p;                             // delete by the denominator
  return pwr;
}