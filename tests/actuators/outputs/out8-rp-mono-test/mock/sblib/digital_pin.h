/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
#ifndef OUT8_TEST_DIGITAL_H_
#define OUT8_TEST_DIGITAL_H_
#include "../model.h"
constexpr int OUTPUT = 1;
inline void pinMode(int pin, int)
{
    if (model::function[26] != 4 || model::slices[5].level[0] != 0)
    {
        throw std::runtime_error("selector configured before common gate became inactive");
    }
    model::direction[pin] = true;
    model::function[pin] = 0;
}
inline void digitalWrite(int pin, bool value)
{
    model::pins[pin] = value;
}
inline unsigned int digitalRead(int pin)
{
    return model::pins[pin];
}
inline unsigned int digitalPinToBitMask(unsigned int pin)
{
    return 1u << (pin % 32);
}
inline unsigned int digitalPinToPort(unsigned int pin)
{
    return pin / 32;
}
inline void portWriteMask(unsigned int port, unsigned int mask, bool value)
{
    for (unsigned int bit = 0; bit < 32 && port * 32 + bit < 48; bit++)
    {
        if (mask & (1u << bit))
        {
            model::pins[port * 32 + bit] = value;
        }
    }
}
#endif
