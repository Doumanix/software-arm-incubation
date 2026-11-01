/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
#include "../model.h"
using pwm_config = model::Config;
inline unsigned int pwm_gpio_to_slice_num(unsigned int pin)
{
    return (pin >> 1) & 7;
}
inline pwm_config pwm_get_default_config()
{
    return {};
}
inline void pwm_config_set_clkdiv_int_frac4(pwm_config* cfg, unsigned int integer, unsigned int fraction)
{
    cfg->divider16 = integer * 16 + fraction;
}
inline void pwm_config_set_wrap(pwm_config* cfg, unsigned int value)
{
    cfg->wrap = value;
}
inline void pwm_config_set_output_polarity(pwm_config* cfg, bool a, bool b)
{
    cfg->invertA = a;
    cfg->invertB = b;
}
inline void pwm_init(unsigned int slice, const pwm_config* cfg, bool enabled)
{
    model::slices[slice].config = *cfg;
    model::slices[slice].enabled = enabled;
    model::slices[slice].initializations++;
}
inline void pwm_set_gpio_level(unsigned int pin, unsigned int level)
{
    model::slices[pwm_gpio_to_slice_num(pin)].level[pin & 1] = level;
}
inline void pwm_set_enabled(unsigned int slice, bool value)
{
    model::slices[slice].enabled = value;
}
