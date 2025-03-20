#ifndef _PID_H_
#define _PID_H_
#include <stdint.h>

class PID
{
  public:
    PID() : temp_h0(0), temp_h1(0), pid_iterate(false), i_summ(0), power(0), Kp(3000), Ki(50), Kd(2048) {}

    void resetPID(int temp = -1); // reset PID algorithm history parameters
    // Calculate the power to be applied
    int32_t reqPower(int temp_set, int temp_curr);
    int16_t changePID(uint8_t p, int k); // set or get (if parameter < 0) PID parameter
  private:
    void debugPID(int t_set, int t_curr, int32_t kp, int32_t ki, int32_t kd, int32_t delta_p);
    int16_t temp_h0, temp_h1;         // previously measured temperature
    bool pid_iterate;                 // Whether the iterative process is used
    int32_t i_summ;                   // Ki summary multiplied by denominator
    int32_t power;                    // The power iterative multiplied by denominator
    int32_t Kp, Ki, Kd;               // The PID algorithm coefficients multiplied by denominator
    const uint8_t denominator_p = 11; // The common coefficient denominator power of 2 (11 means divide by 2048)
};
#endif
