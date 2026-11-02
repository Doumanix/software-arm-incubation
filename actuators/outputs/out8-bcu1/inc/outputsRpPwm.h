/**
 * @brief Common active-LOW hardware PWM gate for the out8 RP test profile.
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#ifndef OUT8_OUTPUTS_RP_PWM_H_
#define OUT8_OUTPUTS_RP_PWM_H_

/**
 * @brief Hardware PWM of the common active-LOW gate; idles HIGH, so the gate is inactive.
 */
class Out8RpPwm
{
public:
    /** @brief Starts the PWM with the gate inactive (constant HIGH). */
    static void begin();

    /** @brief Pull-in pulse: LOW except for one counter tick per period. */
    static void pullIn();

    /** @brief Holding level: LOW for PWM_DUTY ticks per period. */
    static void hold();

    /** @brief Gate inactive (constant HIGH). */
    static void stop();
};

#endif
