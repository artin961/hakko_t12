#ifndef _EMP_AVARAGE_H_
#define _EMP_AVARAGE_H_
#include <Arduino.h>
#include "Config.h"

// ------------------------------------------ class Exponential Moving Average ----------------------------------------------------
class EMP_AVERAGE
{
public:
    EMP_AVERAGE(uint8_t h_length = 8) : emp_k(h_length), emp_data(0) {}
    void length(uint8_t h_length)
    {
        emp_k = h_length;
        emp_data = 0;
    }
    void reset() { emp_data = 0; }
    int32_t average(int32_t value);
    void update(int32_t value);
    int32_t read() const;

private:
    volatile uint8_t emp_k;
    volatile uint32_t emp_data;
};

// ------------------------------------------ class HISTORY ----------------------------------------------------
class HISTORY
{
public:
    HISTORY() : len(0), index(0) {}
    void init()
    {
        len = 0;
        index = 0;
    }
    uint16_t read() const;
    uint16_t top() const { return queue[0]; }
    void update(uint16_t item);
    uint16_t average() const;
    float dispersion() const;

private:
    volatile uint16_t queue[H_LENGTH] = {0};
    volatile uint8_t len;
    volatile uint8_t index;
};
#endif
