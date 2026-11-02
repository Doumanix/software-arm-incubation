in4-bcu2
========

The emulated device is a ABB TSU/4.2 with 4 digital inputs.

Please use the ABB TSU/4.2 VD for ETS configuration from this site:
http://www.knx-gebaeudesysteme.de/sto_g/_All/ETS/TSU42.VD1

Used IO Ports
-------------

P2.0 - P2.3  Input pins

P0.0         Reset
P0.10        LPC-Link clock
P1.3         LPC-Link data I/O
P1.5         EIB programming mode led and button
P1.8         EIB input
P1.10        EIB output

Build
-----

CMake presets (in addition to the MCUXpresso project):

- `arm`: LPC1115, Debug and Release (workflow `build-arm-all`)
- `rp2350-pico2`: RP2350 on the Pico 2 test board, sblib RP2350 port with Pico SDK 2.2.0
  from `PICO_SDK_PATH` (workflow `build-rp2350-pico2-all`)

Pico 2 test board (RP2350): input channel 1..4 on GPIO2, GPIO3, GPIO6, GPIO7 (pull-up, contact
to GND), LED on GPIO25 (on-board LED). KNX and programming button as defined by the sblib board
(`gpio_rp2350_pico2_testboard.h`). No serial debug output on the RP2350 yet.
