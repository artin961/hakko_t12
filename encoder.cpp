#include "Encoder.h"

//------------------------------------------ class BUTTON ------------------------------------------------------
BUTTON::BUTTON(uint16_t to) : over_press(to) {}

void BUTTON::init()
{
  ENCODER_DDR &= ~(ENCODER_BUTTON_BITMASK);
  ENCODER_PORT |= ENCODER_BUTTON_BITMASK; // Enable pull-up
}

uint8_t BUTTON::buttonCheck()
{
  uint32_t now = millis();
  if (now < b_check)
    return 0;
  b_check = now + b_check_period;

  uint8_t s = (!(ENCODER_PIN & ENCODER_BUTTON_BITMASK)) ? (trigger_on << 1) : 0;
  b_on = (b_on) ? (avg.average(s) >= trigger_off) : (avg.average(s) > trigger_on);

  if (!b_on)
  {
    if (bpt && !i_b_rel)
    {
      uint32_t press_duration = now - bpt;
      bpt = 0;
      i_b_rel = false;
      return (press_duration < over_press) ? 1 : 0; // Short press
    }
    return 0;
  }

  if (bpt == 0 || now - bpt > over_press)
  {
    bpt = now;
  }
  else if (now - bpt > long_press)
  {
    return (tick_period) ? buttonTick() : (i_b_rel = true, 2); // Long press
  }

  return 0;
}

void BUTTON::setTimeout(uint16_t to)
{
  over_press = constrain(to, def_over_press, 5000);
  tick_period = 0;
}

bool BUTTON::setTick(uint16_t to)
{
  tick_period = (to > 0) ? constrain(to, 100, 300) : 0;
  over_press = (tick_period) ? 16000 : def_over_press;
  return tick_period > 0;
}

uint8_t BUTTON::buttonTick()
{
  uint32_t now_t = millis();
  if (now_t - tick_time > tick_period)
  {
    tick_time = now_t;
    return (bpt != 0);
  }
  return 0;
}

//------------------------------------------ class ENCODER ------------------------------------------------------
RENC::RENC(int16_t init_pos) : BUTTON(), pos(init_pos), min_pos(-32767), max_pos(32766) {}

void RENC::init()
{
  BUTTON::init();

  ENCODER_DDR &= ~(ENCODER_DATA_BITMASK | ENCODER_CLOCK_BITMASK);
  ENCODER_PORT |= ENCODER_DATA_BITMASK | ENCODER_CLOCK_BITMASK;

  EICRA = (EICRA & ~(1 << ISC11)) | (1 << ISC10); // Trigger on logical change
  EIMSK |= (1 << INT1);                           // Enable external interrupt
}

bool RENC::write(int16_t init_pos)
{
  if (init_pos >= min_pos && init_pos <= max_pos)
  {
    pos = init_pos;
    return true;
  }
  return false;
}

void RENC::reset(int16_t init_pos, int16_t low, int16_t upp, uint8_t inc, uint8_t fast_inc, bool looped)
{
  min_pos = low;
  max_pos = upp;
  if (!write(init_pos))
    pos = min_pos;
  increment = inc;
  fast_increment = (fast_inc > increment) ? fast_inc : increment;
  is_looped = looped;
}

void RENC::encoderIntr()
{
  uint32_t now_t = millis();
  bool rUp = ENCODER_PIN & ENCODER_CLOCK_BITMASK;

  if (!rUp)
  {
    if (!pt || now_t - pt > over_press)
    {
      pt = now_t;
      ch_b = ENCODER_PIN & ENCODER_DATA_BITMASK;
    }
  }
  else if (pt)
  {
    uint8_t inc = ((now_t - changed) < fast_timeout) ? fast_increment : increment;
    changed = now_t;
    pos += ch_b ? -inc : inc;

    if (pos > max_pos)
      pos = (is_looped) ? min_pos : max_pos;
    if (pos < min_pos)
      pos = (is_looped) ? max_pos : min_pos;
    pt = 0;
  }
}
