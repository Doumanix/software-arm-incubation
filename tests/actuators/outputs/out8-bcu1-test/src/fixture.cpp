/*
 *  fixture.cpp - Resets the emulated power-on state before each test case
 *
 *  Copyright (c) 2026 Doumanix <doumanix@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "out8.h"
#include "app_out8.h"
#include <cstring>
#include <new>

extern void resetOut8Application();
extern unsigned int wfiSystemTimeInc;
extern void setMillis(unsigned int newSystemTime);

Out8TestFixture::Out8TestFixture()
{
    // Reset once at the case boundary, including after a Catch REQUIRE abort.
    setMillis(0);
    wfiSystemTimeInc = 0;
    std::memset(&_LPC_TMR16B0, 0, sizeof(_LPC_TMR16B0));
    std::memset(&_LPC_TMR16B1, 0, sizeof(_LPC_TMR16B1));
    std::memset(&_LPC_TMR32B0, 0, sizeof(_LPC_TMR32B0));
    std::memset(&_LPC_TMR32B1, 0, sizeof(_LPC_TMR32B1));
    std::memset(&_LPC_GPIO0, 0, sizeof(_LPC_GPIO0));
    std::memset(&_LPC_GPIO1, 0, sizeof(_LPC_GPIO1));
    std::memset(&_LPC_GPIO2, 0, sizeof(_LPC_GPIO2));
    std::memset(&_LPC_GPIO3, 0, sizeof(_LPC_GPIO3));
    std::memset(&_LPC_IOCON, 0, sizeof(_LPC_IOCON));
    std::memset(&_LPC_SYSCON, 0, sizeof(_LPC_SYSCON));
    std::memset(&_NVIC, 0, sizeof(_NVIC));
    SystemCoreClock = 48000000;
    _refState = {};
    _stepState = {};
    resetOut8Application();

    // Reuse allocations owned by the global BCU. Reconstruct only allocation-free
    // subobjects, preserving the bus callback and its reference to that same BCU.
    auto* objects = static_cast<ComObjectsBCU1*>(bcu.comObjects);
    objects->~ComObjectsBCU1();
    new (objects) ComObjectsBCU1(&bcu);
    auto* bus = bcu.bus;
    auto* callback = bus->callBack;
    bus->~Bus();
    new (bus) Bus(bcu.addrTables, timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, TIMER_MATCH_MAT0, callback);
    std::memset(bcu.userRam->userRamData, 0, bcu.userRam->sizeTotal + bcu.userRam->shadowSize);
    bcu.userRam->_status = 0;
    bcu.userRam->_runState = 0;
    std::memset(bcu.userEeprom->userEepromData, 0, bcu.userEeprom->sizeTotal);
    bcu.userEeprom->modified(false);
    bcu.userEeprom->writeUserEepromTime = 0;
    std::memset(bcu.sendTelegram, 0, TelegramBufferSize);
    std::memset(bcu.sendConnectedTelegram, 0, TelegramBufferSize);
    std::memset(bcu.sendConnectedTelegram2, 0, TelegramBufferSize);
    bcu.restartType = RestartType::None;
    bcu.restartSendDisconnect = false;
    bcu.restartTimeout.stop();
    bcu.sendGrpTelEnabled = false;
    bcu.groupTelSent = 0;
    bcu.groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS;
    // executeTestOnBcu resets flash, initializes transport/bus and runs setup()
    // with each case's EEPROM. Keep all telegram and output assertions there.
}
