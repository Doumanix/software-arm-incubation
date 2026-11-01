# RP out8 monostable host checks

Configure this directory with native CMake/Ninja and
`-DSB_PATH_TO_SBLIB_REPO=<fixed lib checkout>`, then build and run CTest.
Use an isolated build directory or a cmake-build-* directory in a checkout.

The executable compiles the actual Outputs and Out8RpPwm implementation against
GPIO/PWM register and pad models. It also uses the library's real Timeout class.
Five groups cover configuration, exact timeout boundaries and repeated ON, eight
independent channel bits/inversion, shared-gate and serial-power behavior, wrap,
OFF during pull-in and the zero sentinel. The hardware API stubs record the
configuration passed by the real driver, levels, enabled slices and pad mux.

The model observes register requests; it does not simulate the Pico SDK compare
double-buffering, analog power, bus load or actual waveform timing. Physical
signal acceptance remains separate. No fixture communicates with hardware.
