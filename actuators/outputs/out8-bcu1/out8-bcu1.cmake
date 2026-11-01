set(OUT8_BCU1_SRC
    inc/app_main.h
    inc/app_out8.h
    inc/com_objs.h
    inc/config.h
    inc/outputs.h
    inc/outputsBiStable.h
    src/app_main.cpp
    src/app_out8.cpp
    src/cr_startup_lpc11xx.cpp
    src/outputs.cpp
    src/outputsBiStable.cpp
)

# In MCUXpresso as linked folders src/bus_voltage and src/hand-actuation
# (always compiled, independent of the build variant)
set(OUT8_BCU1_COMMON_SRC
    bus_voltage/app_nov_settings.cpp
    bus_voltage/app_nov_settings.h
    bus_voltage/bus_voltage.cpp
    bus_voltage/bus_voltage.h
    hand-actuation/hand_actuation.cpp
    hand-actuation/hand_actuation.h
)
