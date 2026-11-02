/*
 *  config.h - Input and LED pins of in4-bcu2 per platform and board.
 *
 *  @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef IN4_BCU2_CONFIG_H_
#define IN4_BCU2_CONFIG_H_

#include "sblib/io_pin_names.h"

#if defined(SBLIB_BOARD_RP2350_PICO2_TESTBOARD)
// Pico 2 test board (RP2350): test profile, no input PCB wiring implied.
// Channel 1..4 on GPIO2/3/6/7 (input with pull-up, contact to GND), LED on the Pico 2 on-board LED.
#   define PIO_LED PIN_RUN
static const int inputPins[] = {GPIO2, GPIO3, GPIO6, GPIO7};
#elif defined(SBLIB_PLATFORM_RP2350)
#   error "in4-bcu2: no input pins for this RP2350 board"
#else
// Digital pin for LED
#   define PIO_LED PIO0_7

// Input pins
static const int inputPins[] = {PIO2_3, PIO2_2, PIO2_1, PIO2_0};
#endif

#endif /* IN4_BCU2_CONFIG_H_ */
