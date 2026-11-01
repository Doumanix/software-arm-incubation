/*
 *  app_fixture.cpp - Builds the out8 application for the host tests
 *
 *  Copyright (C) 2026 Doumanix <doumanix@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

// Compile the unchanged application in this test translation unit so its private
// channel timers can be reset without adding a firmware API.
#include "app_out8.cpp"
#include <new>

void resetOut8Application()
{
    for (auto& timer : channel_timeout)
    {
        timer.On.stop();
        timer.Off.stop();
    }
    relays.~Outputs();
    new (&relays) Outputs();
}
