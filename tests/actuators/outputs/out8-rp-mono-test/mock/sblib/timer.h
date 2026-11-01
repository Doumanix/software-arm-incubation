/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
#ifndef OUT8_TEST_TIMER_H_
#define OUT8_TEST_TIMER_H_
#include "../model.h"
#define ALWAYS_INLINE inline
inline unsigned int millis()
{
    return model::now;
}
inline void delay(unsigned int ms)
{
    model::now += ms;
    model::delayed += ms;
}
#endif
