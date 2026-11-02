/**
 * @brief RP hardware PWM equivalent of the LPC out8 common gate.
 * @author Doumanix <doumanix@gmx.de>
 * Copyright (C) 2026 Doumanix <doumanix@gmx.de>
 * @par License
 * GNU General Public License version 3.
 */
#include "outputs.h"
#include "sblib/utils.h"
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

namespace
{
    // LPC counts 0..MR3 inclusive at 100 kHz. Scale by 16 to keep the
    // RP divider within range while retaining the same period and low time.
    constexpr uint32_t counterScale = 16;
    constexpr uint32_t counterHz = 100000 * counterScale;
    constexpr uint32_t wrap = (PWM_PERIOD + 1) * counterScale - 1;

    void setLowTicks(const uint32_t ticks)
    {
        pwm_set_gpio_level(OUT8_RP_PWM_PIN, ticks * counterScale);
    }
} // namespace

void Out8RpPwm::begin()
{
    // Preload the inactive HIGH level before connecting the PWM to the pad.
    gpio_init(OUT8_RP_PWM_PIN);
    gpio_put(OUT8_RP_PWM_PIN, true);
    gpio_set_dir(OUT8_RP_PWM_PIN, true);
    const uint32_t slice = pwm_gpio_to_slice_num(OUT8_RP_PWM_PIN);
    pwm_config config = pwm_get_default_config();
    const uint64_t divider16 = (static_cast<uint64_t>(clock_get_hz(clk_sys)) * 16u + counterHz / 2u) / counterHz;
    if (divider16 < 16 || divider16 > 4095)
    {
        fatalError();
    }
    pwm_config_set_clkdiv_int_frac4(&config, divider16 / 16u, divider16 % 16u);
    pwm_config_set_wrap(&config, wrap);
    pwm_config_set_output_polarity(&config, true, false);
    pwm_init(slice, &config, false);
    setLowTicks(0);
    gpio_set_function(OUT8_RP_PWM_PIN, GPIO_FUNC_PWM);
    pwm_set_enabled(slice, true);
}

void Out8RpPwm::pullIn()
{
    // Like LPC MAT2=MAT3: one timer tick HIGH per cycle, otherwise LOW.
    setLowTicks(PWM_PERIOD);
}

void Out8RpPwm::hold()
{
    setLowTicks(PWM_DUTY);
}

void Out8RpPwm::stop()
{
    setLowTicks(0); // Constant HIGH: inactive common gate.
}
