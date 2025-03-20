#include "EmpAvarage.h"
// ------------------------------------------ class Exponential Moving Average ----------------------------------------------------
int32_t EMP_AVERAGE::average(int32_t value)
{
    update(value);
    return read();
}

void EMP_AVERAGE::update(int32_t value)
{
    uint8_t round_v = emp_k >> 1;
    emp_data += value - ((emp_data + round_v) / emp_k);
}

int32_t EMP_AVERAGE::read() const
{
    return (emp_data + (emp_k >> 1)) / emp_k;
}

//------------------------------------------ class HISTORY ----------------------------------------------------
void HISTORY::update(uint16_t item)
{
    if (len < H_LENGTH)
    {
        queue[len++] = item;
    }
    else
    {
        queue[index] = item;
        index = (index + 1) % H_LENGTH; // Efficient ring buffer
    }
}

uint16_t HISTORY::read() const
{
    return (len > 0) ? queue[(index > 0) ? index - 1 : H_LENGTH - 1] : 0;
}

uint16_t HISTORY::average() const
{
    if (len == 0)
        return 0;
    if (len == 1)
        return queue[0];

    uint32_t sum = 0;
    for (uint8_t i = 0; i < len; ++i)
        sum += queue[i];

    return (sum + (len >> 1)) / len; // Optimized rounding
}

float HISTORY::dispersion() const
{
    if (len < 3)
        return 1000.0f;

    uint32_t avg = average();
    uint32_t sum = 0;

    for (uint8_t i = 0; i < len; ++i)
    {
        int32_t diff = queue[i] - avg;
        sum += diff * diff;
    }

    return static_cast<float>(sum) / len;
}
