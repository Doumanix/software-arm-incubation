/**
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#include "../model.h"
using pwm_config = model::Config;
inline uint32_t pwm_gpio_to_slice_num(uint32_t pin)
{
    return (pin >> 1) & 7;
}
inline pwm_config pwm_get_default_config()
{
    return {};
}
inline void pwm_config_set_clkdiv_int_frac4(pwm_config* cfg, uint32_t integer, uint32_t fraction)
{
    cfg->divider16 = integer * 16 + fraction;
}
inline void pwm_config_set_wrap(pwm_config* cfg, uint32_t value)
{
    cfg->wrap = value;
}
inline void pwm_config_set_output_polarity(pwm_config* cfg, bool a, bool b)
{
    cfg->invertA = a;
    cfg->invertB = b;
}
inline void pwm_init(uint32_t slice, const pwm_config* cfg, bool enabled)
{
    model::slices[slice].config = *cfg;
    model::slices[slice].enabled = enabled;
    model::slices[slice].initializations++;
}
inline void pwm_set_gpio_level(uint32_t pin, uint32_t level)
{
    model::slices[pwm_gpio_to_slice_num(pin)].level[pin & 1] = level;
}
inline void pwm_set_enabled(uint32_t slice, bool value)
{
    model::slices[slice].enabled = value;
}
