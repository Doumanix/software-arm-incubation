/**
 * @brief Executes the real out8 channel/PWM code against observable register/pad models.
 * Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3.
 */
#include "outputs.h"
#include "model.h"
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
    uint32_t assertions = 0;
    void require(bool condition, const char* text)
    {
        assertions++;
        if (!condition)
        {
            throw std::runtime_error(text);
        }
    }
    uint32_t lowTicks()
    {
        return model::slices[5].level[0];
    }
    uint32_t selected()
    {
        uint32_t result = 0;
        for (uint32_t channel = 0; channel < 8; channel++)
        {
            if (model::pins[outputPins[channel]])
            {
                result |= 1u << channel;
            }
        }
        return result;
    }
    void reset(uint32_t initial = 0, uint32_t inverted = 0, uint32_t now = 0)
    {
        for (auto& slice: model::slices)
        {
            slice = {};
        }
        for (uint32_t pin = 0; pin < 48; pin++)
        {
            model::pins[pin] = false;
            model::function[pin] = -1;
        }
        model::now = now;
        model::clock = 150000000;
        model::delayed = 0;
        relays = Outputs();
        relays.setupOutputs(outputPins, NO_OF_OUTPUTS);
        relays.begin(initial, inverted, NO_OF_CHANNELS);
        relays.updateOutputs();
    }
    void configuration()
    {
        reset();
        const auto& slice = model::slices[5];
        require(slice.enabled, "hardware slice not running");
        require(slice.config.wrap == 1375, "period must be 86 scaled ticks");
        require(slice.config.divider16 == 1500, "clock-derived divider");
        require(slice.config.invertA && !slice.config.invertB, "active-LOW common gate");
        require(model::function[26] == 4 && model::pins[26], "gate pad starts inactive HIGH");
        require(selected() == 0 && lowTicks() == 0, "init OFF");
        for (uint32_t channel = 0; channel < 8; channel++)
        {
            require(model::function[outputPins[channel]] == 0, "channel must remain SIO");
        }
        for (uint32_t sliceNumber = 0; sliceNumber < 12; sliceNumber++)
        {
            if (sliceNumber != 5)
            {
                require(model::slices[sliceNumber].initializations == 0, "unowned slice modified");
            }
        }
    }
    void boundaryAndRepetition()
    {
        reset();
        relays.setChannel(0);
        require(relays.updateOutput(0), "ON must report supply transition");
        require(selected() == 1 && lowTicks() == 1360, "pull-in value/pin");
        model::now = 19;
        relays.checkPWM();
        require(lowTicks() == 1360, "must pull until 20 ms");
        relays.setChannel(0);
        require(!relays.updateOutput(0), "same ON must not retrigger");
        model::now = 20;
        relays.checkPWM();
        require(lowTicks() == 352, "must hold at 20 ms, not 39 ms");
        model::now = 21;
        relays.checkPWM();
        require(lowTicks() == 352, "stable hold");
        relays.clearChannel(0);
        relays.updateOutput(0);
        require(selected() == 0 && lowTicks() == 0, "last OFF must disable common gate");
    }
    void eightChannelsAndInversion()
    {
        for (uint32_t channel = 0; channel < 8; channel++)
        {
            reset();
            relays.setChannel(channel);
            relays.updateOutputs();
            require(selected() == (1u << channel), "channel bit/pin isolation");
            model::now = 20;
            relays.checkPWM();
            require(lowTicks() == 352, "independent channel hold");
            relays.clearChannel(channel);
            relays.updateOutputs();
            require(selected() == 0 && lowTicks() == 0, "channel OFF isolation");
        }
        reset(0xa5, 0x5a);
        require(selected() == 0xff, "initial logical/inversion XOR");
        require(relays.channel(0) == 1 && relays.channel(1) == 0, "inversion must not corrupt logical state");
        for (uint32_t channel = 0; channel < 8; channel++)
        {
            relays.updateChannel(channel, (0x5a >> channel) & 1);
        }
        relays.updateOutputs();
        require(selected() == 0 && lowTicks() == 0, "physical OFF after inverted commands");
    }
    void commonGateAndSerialPower()
    {
        reset();
        relays.setChannel(0);
        relays.updateOutputs();
        model::now = 20;
        relays.checkPWM();
        model::now = 25;
        relays.setChannel(7);
        relays.updateOutputs();
        require(selected() == 0x81 && lowTicks() == 1360, "new channel boosts shared gate");
        model::now = 44;
        relays.checkPWM();
        require(lowTicks() == 1360, "new common deadline 45 ms");
        model::now = 45;
        relays.checkPWM();
        require(lowTicks() == 352, "shared hold deadline");
        reset();
        for (uint32_t channel = 0; channel < 8; channel++)
        {
            relays.setChannel(channel);
        }
        relays.updateOutputs(100);
        require(selected() == 0xff && model::delayed == 700, "existing seven inter-channel power delays preserved");
        model::now += 20;
        relays.checkPWM();
        require(lowTicks() == 352, "all eight hold");
    }
    void wrapAndStop()
    {
        reset(0, 0, 0xfffffff0u);
        relays.setChannel(4);
        relays.updateOutputs();
        model::now = 3;
        relays.checkPWM();
        require(lowTicks() == 1360, "wrap: 19 ms");
        model::now = 4;
        relays.checkPWM();
        require(lowTicks() == 352, "wrap: 20 ms");
        Out8RpPwm::stop();
        require(lowTicks() == 0, "explicit stop");
        reset();
        model::clock = 125000000;
        Out8RpPwm::begin();
        require(model::slices[5].config.divider16 == 1250, "125 MHz clock divider");
        reset();
        relays.setChannel(0);
        relays.updateOutputs();
        model::now = 5;
        relays.clearChannel(0);
        relays.updateOutputs();
        require(selected() == 0 && lowTicks() == 0, "OFF during pull-in");
        model::now = 25;
        relays.checkPWM();
        require(lowTicks() == 0, "no stale hold after OFF");
        reset(0, 0, 0xffffffecu);
        relays.setChannel(0);
        relays.updateOutputs();
        model::now = 0;
        relays.checkPWM();
        require(lowTicks() == 1360, "existing Timeout zero sentinel defers by one tick");
        model::now = 1;
        relays.checkPWM();
        require(lowTicks() == 352, "sentinel expires after one additional ms");
        reset();
        model::clock = 1000;
        bool rejected = false;
        try
        {
            Out8RpPwm::begin();
        }
        catch (const std::runtime_error&)
        {
            rejected = true;
        }
        require(rejected && model::pins[26], "invalid clock must leave gate inactive");
    }
} // namespace

int main()
{
    try
    {
        configuration();
        boundaryAndRepetition();
        eightChannelsAndInversion();
        commonGateAndSerialPower();
        wrapAndStop();
        std::cout << "OUT8_MONO_HOST_PASS: 5 groups, " << assertions << " assertions\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "OUT8_MONO_HOST_FAIL: " << error.what() << '\n';
        return 1;
    }
}
