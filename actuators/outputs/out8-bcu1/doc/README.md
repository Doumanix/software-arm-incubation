# RP monostable test profile

`out8-bcu1_o8_pico2tb_mono` is an explicit Pico 2 test-signal profile selected by
OUT8_RP_MONOSTABLE. It is not a verified relay-PCB pinout. The BI_STABLE profile
is retained. HAND_ACTUATION, BUSFAIL and ZERO_DETECT are rejected in both RP
profiles; selecting both or neither output backend is a compile error.

Eight active-HIGH channel-select signals use GPIO2, 3, 6, 7, 10, 11, 12 and 13.
The additional common gate is GPIO26, active LOW, PWM slice 5 channel A. GPIO10/11
stay SIO even though their alternative PWM functions alias that slice. Slice 5/B
has no application owner. No PWM IRQ, DMA, PIO state machine or software bit-edge
loop is allocated. The KNX PIO path and reserved GPIO4/5, 8/9, 22, 25, 27/28 remain
independent. GPIO23/24/29 belong to the Pico board and are not used here.

The LPC contract comes from outputs.cpp and the Timer::pwmEnable documentation:
100 kHz timer input; MR3=85 resets after counting 0..85 inclusive; MAT2=22 makes
the MCU PWM pin LOW for 22 ticks and HIGH for 64. MAT2=85 during pull-in gives
85 LOW ticks and one HIGH tick, rather than literal 100% DC. See NXP UM10398,
single-edge controlled PWM rules (19.7.13 in revision 12.4). Do not infer coil
power from the approximate legacy 1.2 kHz/25% comments or unverified board wiring.

The RP counter runs at 1.6 MHz with wrap 1375 (16-times scaling), inverted A,
CC_A=352 for holding and CC_A=1360 for pull-in. At the normal 150 MHz clock,
divider=93+12/16; period=860 us (1162.7907 Hz), holding LOW=220 us / HIGH=640 us,
pull-in LOW=850 us / HIGH=10 us. Idle/OFF CC_A=0 is constant HIGH. The divider is
rounded from the actual clk_sys frequency; unsupported ranges fail with the gate
preloaded HIGH. It is not a dynamically clock-changing PWM driver.

Logical channel state is separate from physical selection: selected bits are
logical XOR EEPROM inversion. Only a real newly selected ON starts the existing
20-ms Timeout. Repeated ON/app loops do not retrigger it. A new active channel
restarts the shared pull-in phase for every selected channel, preserving the LPC
common-supply contract. Channels are independently selectable; the supply PWM
phase is deliberately shared. The existing 100-ms inter-channel power sequencing
is preserved (20-ms check plus the remaining delay). Hardware PWM keeps running
without software bit-edge timing during those existing delays.

At Timeout expiry the common gate changes to holding. If all physical selects
are OFF it stops instead; OFF during pull-in cannot later restore holding. The
existing Timeout wrap/zero-sentinel rule is retained (a zero deadline advances
one millisecond). RP compare writes are double-buffered until the next PWM wrap;
the physical waveform can change up to 860 us after the software register write.

Init preloads the common gate HIGH before connecting the PWM pad. All channel
pins start LOW; initial/inverted parameter state is then applied through the same
channel methods. stopApplication stops the common gate before clearing selects.
There is no new library HAL or change to the LPC and bistable code paths.

Build with the RP preset and fixed sblib checkout, PIO ON, then target out8-bcu1_all
or out8-bcu1_o8_pico2tb_mono in Release. A mono map is generated beside its ELF.
Native register-model tests are in tests/actuators/outputs/out8-rp-mono-test.

Hardware signal acceptance requires a separate installed-image/run/GA/pin handoff.
No flash, ETS operation, rewiring, load or electrical relay approval is implied.
