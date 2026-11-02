/**
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#include "../model.h"
constexpr int clk_sys = 0;
inline uint32_t clock_get_hz(int)
{
    return model::clock;
}
