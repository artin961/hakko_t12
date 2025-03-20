#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "EmpAvarage.h"
#include "config.h"

//------------------------------------------ class BUTTON ------------------------------------------------------
class BUTTON
{
  public:
    explicit BUTTON(uint16_t timeout_ms = 3000);
    uint8_t buttonCheck();
    void setTimeout(uint16_t to = 3000);
    bool setTick(uint16_t to);
    void init();

  private:
    uint8_t buttonTick();
    EMP_AVERAGE avg; // Average button readings
    uint16_t over_press;
    volatile bool i_b_rel = false;
    bool b_on = false;
    uint32_t bpt = 0, b_check = 0, tick_time = 0;
    uint16_t tick_period = 0;
    static constexpr uint8_t trigger_on = 60;
    static constexpr uint8_t trigger_off = 30;
    static constexpr uint8_t avg_length = 4;
    static constexpr uint8_t b_check_period = 20;
    static constexpr uint16_t long_press = 1500;
    static constexpr uint16_t def_over_press = 2500;
};

//------------------------------------------ class ENCODER ------------------------------------------------------
class RENC : public BUTTON
{
  public:
    explicit RENC(int16_t init_pos = 0);
    void init();
    void set_increment(uint8_t inc) {
      increment = inc;
    }
    uint8_t get_increment() const {
      return increment;
    }
    int16_t read() const {
      return pos;
    }
    void reset(int16_t init_pos, int16_t low, int16_t upp, uint8_t inc = 1, uint8_t fast_inc = 0, bool looped = false);
    bool write(int16_t init_pos);
    void encoderIntr();

  private:
    volatile int16_t pos;
    int32_t min_pos;
    int32_t max_pos;
    bool is_looped = false;
    uint8_t increment = 1, fast_increment = 0;
    volatile uint32_t pt = 0, changed = 0;
    volatile bool ch_b = false;
    static constexpr uint16_t fast_timeout = 300;
    static constexpr uint16_t over_press = 1000;
};

#endif
