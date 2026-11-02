/**
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
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
inline uint32_t digitalRead(int pin)
{
    return model::pins[pin];
}
inline uint32_t digitalPinToBitMask(uint32_t pin)
{
    return 1u << (pin % 32);
}
inline uint32_t digitalPinToPort(uint32_t pin)
{
    return pin / 32;
}
inline void portWriteMask(uint32_t port, uint32_t mask, bool value)
{
    for (uint32_t bit = 0; bit < 32 && port * 32 + bit < 48; bit++)
    {
        if (mask & (1u << bit))
        {
            model::pins[port * 32 + bit] = value;
        }
    }
}
#endif
