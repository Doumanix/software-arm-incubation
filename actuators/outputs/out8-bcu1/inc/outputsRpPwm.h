/**
 * @brief Common active-LOW hardware PWM gate for the out8 RP test profile.
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#ifndef OUT8_OUTPUTS_RP_PWM_H_
#define OUT8_OUTPUTS_RP_PWM_H_

class Out8RpPwm
{
public:
    static void begin();
    static void pullIn();
    static void hold();
    static void stop();
};

#endif
