/**
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#include "../model.h"
constexpr int GPIO_FUNC_PWM = 4;
inline void gpio_init(uint32_t pin)
{
    model::function[pin] = 0;
}
inline void gpio_put(uint32_t pin, bool value)
{
    model::pins[pin] = value;
}
inline void gpio_set_dir(uint32_t pin, bool value)
{
    model::direction[pin] = value;
}
inline void gpio_set_function(uint32_t pin, int value)
{
    model::function[pin] = value;
}
