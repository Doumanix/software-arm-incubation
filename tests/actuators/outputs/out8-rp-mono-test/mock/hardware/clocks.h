/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
#include "../model.h"
constexpr int clk_sys = 0;
inline uint32_t clock_get_hz(int)
{
    return model::clock;
}
