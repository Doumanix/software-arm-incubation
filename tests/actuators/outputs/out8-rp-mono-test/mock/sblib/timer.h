/**
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#ifndef OUT8_TEST_TIMER_H_
#define OUT8_TEST_TIMER_H_
#include "../model.h"
#define ALWAYS_INLINE inline
inline uint32_t millis()
{
    return model::now;
}
inline void delay(uint32_t ms)
{
    model::now += ms;
    model::delayed += ms;
}
#endif
