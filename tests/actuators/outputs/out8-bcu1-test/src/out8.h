/*
 *  out8.cpp - Common functions for the tests of the out8
 *
 *  Copyright (C) 2014-2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef OUT8_H_
#define OUT8_H_

#include "protocol.h"

typedef struct
{
    unsigned int outputs;
    unsigned int pwm_on;
} Out8TestState;

/** @brief Restores the emulated power-on state before each application case. */
struct Out8TestFixture
{
    Out8TestFixture();
};

extern Out8TestState _refState;
extern Out8TestState _stepState;

void _gatherState (Out8TestState * state, Out8TestState * refState);
void _loop(Out8TestState * refState);
void _afterSetup(Telegram* tel, uint16_t telCount);
void _output1Set(Out8TestState * refState);
void _output2Set(Out8TestState * refState);
void _output3Set(Out8TestState * refState);
void _output3Recirculated(Out8TestState* refState);
void _output4Set(Out8TestState * refState);
void _output5Set(Out8TestState * refState);
void _output6Set(Out8TestState * refState);
void _output7Set(Out8TestState * refState);
void _output8Set(Out8TestState * refState);
void _output34Set(Out8TestState * refState);
void _enablePWM(Out8TestState * refState);
void _output1Clear(Out8TestState * refState);
void _output2Clear(Out8TestState * refState);
void _output3Clear(Out8TestState * refState);
void _output4Clear(Out8TestState * refState);
void _output5Clear(Out8TestState * refState);
void _output6Clear(Out8TestState * refState);
void _output7Clear(Out8TestState * refState);
void _output8Clear(Out8TestState * refState);
void _output34Clear(Out8TestState * refState);

void _clearOutputs(void);

/**
 * @brief Runs a test case on the given BCU. Defined in src/protocol.cpp of the sblib-test project,
 *        but not declared in its protocol.h.
 * @details An application test must run on the global @ref bcu of the application. executeTest()
 *          creates a BCU instance of its own, which the application logic does not use, so the
 *          application would not see any telegram of the test case.
 *
 * @param currentBcu - the BCU to run the test case on
 * @param tc - the test case to run
 */
void executeTestOnBcu(BcuDefault* currentBcu, Test_Case* tc);

#endif /* OUT8_H_ */
