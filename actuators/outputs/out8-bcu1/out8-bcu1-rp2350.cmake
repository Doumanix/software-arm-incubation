# out8-bcu1 for the RP2350 (sblib RP2350 port), included by CMakeLists.txt for SBLIB_MCU=RP2350.
#
# Same sources as the LPC1115 build, platform differences only in inc/config.h (output pin table
# per sblib board), inc/outputs.h (GPIO access) and src/app_out8.cpp (random start delay).
# Uses the sblib main(), the Pico SDK startup and linker script and the sblib storage area at
# the end of the flash. Bus voltage monitoring (BUSFAIL) is not available on the RP2350 yet.

# Path to the sblib repository. Relative paths are relative to this directory.
set(SB_PATH_TO_SBLIB_REPO "../../../software-arm-lib" CACHE PATH "Path to the sblib repository (e.g. /home/user/sblib).")
if(IS_ABSOLUTE "${SB_PATH_TO_SBLIB_REPO}")
    set(_sblib_repo "${SB_PATH_TO_SBLIB_REPO}")
else()
    set(_sblib_repo "${CMAKE_CURRENT_SOURCE_DIR}/${SB_PATH_TO_SBLIB_REPO}")
endif()
set(SBLIB_PATH "${_sblib_repo}/sblib" CACHE PATH "Path to the sblib")
set(SB_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../common" CACHE PATH "Path to the incubation 'common' directory")
if(NOT EXISTS "${SBLIB_PATH}")
    message(FATAL_ERROR "sblib project not found. Check setting of SB_PATH_TO_SBLIB_REPO/SBLIB_PATH (${SBLIB_PATH})")
endif()
if(NOT EXISTS "${SB_COMMON_DIR}/hand-actuation")
    message(FATAL_ERROR "common directory not found. Check setting of SB_COMMON_DIR (${SB_COMMON_DIR})")
endif()

include(${_sblib_repo}/cmake/rp2350/pico_sdk_setup.cmake)

project(out8-bcu1 LANGUAGES C CXX ASM VERSION 5.12)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_subdirectory(${SBLIB_PATH} sblib)

set(OUT8_SW_VERSION "${PROJECT_VERSION}" CACHE STRING "Software version used in the artifact names (keep in sync with APP_VERSION in app_main.cpp)")

include(${CMAKE_CURRENT_LIST_DIR}/out8-bcu1.cmake)

# LPC startup code is replaced by the Pico SDK runtime. bus_voltage.cpp uses the LPC11xx ADC.
list(REMOVE_ITEM OUT8_BCU1_SRC src/cr_startup_lpc11xx.cpp)
list(REMOVE_ITEM OUT8_BCU1_COMMON_SRC bus_voltage/bus_voltage.cpp)

set(_common_sources "")
foreach(src ${OUT8_BCU1_COMMON_SRC})
    list(APPEND _common_sources "${SB_COMMON_DIR}/${src}")
endforeach()

# The Pico SDK toolchain does not set CMAKE_SIZE; look for size next to the compiler.
if(NOT CMAKE_SIZE)
    get_filename_component(_toolchain_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_program(CMAKE_SIZE NAMES arm-none-eabi-size HINTS "${_toolchain_bin}" NO_DEFAULT_PATH REQUIRED)
endif()

set(OUT8_TARGETS "")

# Adds one firmware target.
#   name:  variant name, e.g. "o8_pico2tb_bi"
#   ARGN:  variant specific defines (BI_STABLE, ...)
function(out8_rp2350_add_firmware name)
    set(target "${PROJECT_NAME}_${name}")
    add_executable(${target} ${OUT8_BCU1_SRC} ${_common_sources})
    set_target_properties(${target} PROPERTIES OUTPUT_NAME "${target}_v${OUT8_SW_VERSION}")
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${SB_COMMON_DIR}/bus_voltage
        ${SB_COMMON_DIR}/hand-actuation
    )
    target_compile_definitions(${target} PRIVATE OUT8 ${ARGN})
    target_link_libraries(${target} PRIVATE pico_stdlib sblib_rp2350)
    pico_enable_stdio_uart(${target} 0)
    pico_enable_stdio_usb(${target} 0)

    set(_base "$<TARGET_FILE_DIR:${target}>/${target}_v${OUT8_SW_VERSION}")
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> ${_base}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}> ${_base}.bin
        VERBATIM
    )
    set(OUT8_TARGETS ${OUT8_TARGETS} ${target} PARENT_SCOPE)
endfunction()

# Variants per sblib board. Format: "<name>|<define>,<define>,..."
#   pico2tb: Pico 2 test board (output pin table in inc/config.h), bi: bi-stable relays
if(SBLIB_BOARD STREQUAL "pico2_testboard")
    set(OUT8_RP2350_VARIANTS
        "o8_pico2tb_bi|BI_STABLE"
    )
else()
    message(FATAL_ERROR "out8-bcu1: no variants for SBLIB_BOARD=${SBLIB_BOARD} (add an output pin table in inc/config.h)")
endif()

foreach(variant ${OUT8_RP2350_VARIANTS})
    string(REPLACE "|" ";" _parts "${variant}")
    string(REPLACE "," ";" _parts "${_parts}")
    list(GET _parts 0 _name)
    list(REMOVE_AT _parts 0)
    out8_rp2350_add_firmware(${_name} ${_parts})
endforeach()

add_custom_target(${PROJECT_NAME}_all DEPENDS ${OUT8_TARGETS})
message(STATUS "out8-bcu1 RP2350 targets: ${OUT8_TARGETS}")
