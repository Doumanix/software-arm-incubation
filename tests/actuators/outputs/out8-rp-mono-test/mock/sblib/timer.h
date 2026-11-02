/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
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
