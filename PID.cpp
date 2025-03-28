#include "PID.h"

void PID::resetPID(int temp)
{
  temp_h0 = 0;
  power = 0;
  i_summ = 0;
  pid_iterate = false;
  if ((temp > 0) && (temp < 1000))
    temp_h1 = temp;
  else
    temp_h1 = 0;
}
int16_t PID::changePID(uint8_t p, int k)
{
  switch (p)
  {
    case 1:
      if (k >= 0)
        Kp = k;
      return Kp;
    case 2:
      if (k >= 0)
        Ki = k;
      return Ki;
    case 3:
      if (k >= 0)
        Kd = k;
      return Kd;
    default:
      break;
  }
  return 0;
}

int32_t PID::reqPower(int temp_set, int temp_curr)
{
  if (temp_h0 == 0)
  { // First-time execution
    if (!pid_iterate && (temp_set - temp_curr) < 30)
    {
      pid_iterate = true;
      power = 0;
      i_summ = 0;
    }
    i_summ += temp_set - temp_curr;
    power = Kp * (temp_set - temp_curr) + Ki * i_summ;
  }
  else
  {
    int32_t delta_p = Kp * (temp_h1 - temp_curr) +
                      Ki * (temp_set - temp_curr) +
                      Kd * (temp_h0 + temp_curr - 2 * temp_h1);
    power += delta_p;
  }

  if (pid_iterate)
    temp_h0 = temp_h1;
  temp_h1 = temp_curr;

  return (power + (1 << (denominator_p - 1))) >> denominator_p; // Rounded division
}
