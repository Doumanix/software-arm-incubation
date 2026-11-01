/*
 *  out8.cpp - Common functions for the tests of the out8
 *
 *  Copyright (C) 2014-2015 Martin Glueck <martin@mangari.org>
 *  Copyright (c) 2026 Doumanix <doumanix@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "protocol.h"
#include "out8.h"
#include "app_out8.h"
#include "outputs.h"
#include "catch.hpp"
#include "sblib/timer.h"

Out8TestState _refState;
Out8TestState _stepState;

void _clearOutputs(void)
{
    for (int i=0; i < 8; i++)
    {
        digitalWrite(outputPins[i], 0);
    }
}

void _gatherState (Out8TestState * state, Out8TestState * refState)
{
    int i;
    state->outputs = 0;
    state->pwm_on  = timer16_0.match(TIMER_MATCH_MAT2) != PWM_PERIOD;
    for (i=0; i < 8; i++)
    {
        state->outputs |= (digitalRead(outputPins[i]) << i);
    }
    if (refState)
    {
        REQUIRE(state->outputs == refState->outputs);
        REQUIRE(state->pwm_on  == refState->pwm_on);
    }
}

void _loop(Out8TestState * refState)
{
    // A step of a test case runs the application loop once. BcuDefault::loop() queues at most
    // one group telegram, and ComObjects::sendNextGroupTelegram() moves its scan start index
    // one object further on every call without a pending telegram, so a pending com object can
    // need up to objectCount() calls until it is found. The sblib the test cases were generated
    // with restarted the scan at object 0 instead. Run the BCU loop until the pending telegram
    // is queued, as the main loop of the target does between two bus events.
    for (int i = bcu.comObjects->objectCount(); (i >= 0) && (bcu.bus->sendCurTelegram == nullptr); i--)
    {
        bcu.loop();
    }
    loop();
}

void _afterSetup([[maybe_unused]] Telegram* tel, [[maybe_unused]] uint16_t telCount)
{
    // The setup() of the application calls bcu.begin(), which resets the state machine of the
    // bus to Bus::INIT (Bus::begin() -> initState()). On the target the bus timer interrupt
    // moves it out of INIT, in the host emulation that interrupt has to be simulated, as the
    // test harness does in executeTestOnBcu() before it calls setup().
    _LPC_TMR16B1.IR = 4; // match interrupt MR2 of the bus timer
    bcuUnderTest->bus->timerInterruptHandler();
    _LPC_TMR16B1.IR = 4;
    bcuUnderTest->bus->timerInterruptHandler();
    REQUIRE(bcuUnderTest->bus->state == Bus::IDLE);

    // BcuDefault::loop() sends at most one group telegram per DEFAULT_GROUP_TEL_WAIT_MILLIS
    // (35 ms). The steps of the test cases run the application loop once per telegram without
    // letting any time pass, so remove the rate limit to test the application logic only.
    bcuUnderTest->groupTelWaitMillis = 0;
}

void _output1Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x01;
    refState->pwm_on   = 0;
}
void _output2Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x02;
    refState->pwm_on   = 0;
}
void _output3Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x04;
    refState->pwm_on   = 0;
}
void _output4Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x08;
    refState->pwm_on   = 0;
}
void _output5Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x10;
    refState->pwm_on   = 0;
}
void _output6Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x20;
    refState->pwm_on   = 0;
}
void _output7Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x40;
    refState->pwm_on   = 0;
}
void _output8Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x80;
    refState->pwm_on   = 0;
}

void _output34Set(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs |= 0x0C;
    refState->pwm_on   = 0;
}

void _enablePWM(Out8TestState * refState)
{
    _loop(refState);
    refState->pwm_on = 0x01;
}

void _output1Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x01;
}
void _output2Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x02;
}
void _output3Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x04;
}
void _output4Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x08;
}
void _output5Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x10;
}
void _output6Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x20;
}
void _output7Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x40;
}
void _output8Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x80;
}

void _output34Clear(Out8TestState * refState)
{
    _loop(refState);
    refState->outputs &= ~0x0C;
}

// Gira application 206001, 10283110.pdf page 21: a new switching telegram
// is required after the AND feedback input becomes true.
void _output3Recirculated(Out8TestState* refState)
{
    _loop(refState);
    REQUIRE(bcu.comObjects->objectRead(2) == 0);
    REQUIRE((getRelaysState() & 0x04) == 0);
}
