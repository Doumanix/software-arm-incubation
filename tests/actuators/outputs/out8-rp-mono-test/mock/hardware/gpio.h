/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
#include "../model.h"
constexpr int GPIO_FUNC_PWM = 4;
inline void gpio_init(unsigned int pin)
{
    model::function[pin] = 0;
}
inline void gpio_put(unsigned int pin, bool value)
{
    model::pins[pin] = value;
}
inline void gpio_set_dir(unsigned int pin, bool value)
{
    model::direction[pin] = value;
}
inline void gpio_set_function(unsigned int pin, int value)
{
    model::function[pin] = value;
}
