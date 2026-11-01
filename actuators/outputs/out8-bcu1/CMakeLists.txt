cmake_minimum_required(VERSION 3.28)

# Target microcontroller. RP2350: separate build in out8-bcu1-rp2350.cmake (Pico SDK).
set(SBLIB_MCU "LPC1115" CACHE STRING "Target microcontroller (LPC1115, RP2350)")
set_property(CACHE SBLIB_MCU PROPERTY STRINGS LPC1115 RP2350)
if(SBLIB_MCU STREQUAL "RP2350")
    include(${CMAKE_CURRENT_LIST_DIR}/out8-bcu1-rp2350.cmake)
    return()
endif()

project(out8-bcu1 LANGUAGES C CXX VERSION 5.12)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose output from the build system")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Path to the sblib repository. Relative paths are relative to this directory.
set(SB_PATH_TO_SBLIB_REPO "../../../software-arm-lib" CACHE PATH "Path to the sblib repository (e.g. /home/user/sblib).
 This is used to set SBLIB_PATH and SBLPC_DIR based on the expected structure of the sblib repository.")
if(IS_ABSOLUTE "${SB_PATH_TO_SBLIB_REPO}")
    set(_sblib_repo "${SB_PATH_TO_SBLIB_REPO}")
else()
    set(_sblib_repo "${CMAKE_CURRENT_SOURCE_DIR}/${SB_PATH_TO_SBLIB_REPO}")
endif()
set(SBLPC_DIR "${_sblib_repo}/vendor/CMSIS_CORE_LPC11xx" CACHE PATH "Path to the 'CMSIS_CORE_LPC11xx' directory")
set(SBLIB_PATH "${_sblib_repo}/sblib" CACHE PATH "Path to the sblib")
set(SB_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../common" CACHE PATH "Path to the incubation 'common' directory")

# Toolchain file is set in CMakePresets.json or on command line
if(CMAKE_CROSSCOMPILING AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Cross-compiling but CMAKE_TOOLCHAIN_FILE not set. Set in CMakePresets.json or configure with -DCMAKE_TOOLCHAIN_FILE=<path to>/arm-none-eabi_mcuxpresso.cmake")
endif()
if(CMAKE_CROSSCOMPILING AND DEFINED CMAKE_TOOLCHAIN_FILE AND NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    message(FATAL_ERROR "Toolchain file does not exist: ${CMAKE_TOOLCHAIN_FILE}")
endif()
if(NOT EXISTS "${SBLIB_PATH}")
    message(FATAL_ERROR "sblib project not found. Check setting of SB_PATH_TO_SBLIB_REPO/SBLIB_PATH (${SBLIB_PATH})")
endif()
if(NOT EXISTS "${SBLPC_DIR}")
    message(FATAL_ERROR "CMSIS_CORE_LPC11xx project not found. Check setting of SBLPC_DIR (${SBLPC_DIR})")
endif()
if(NOT EXISTS "${SB_COMMON_DIR}/bus_voltage")
    message(FATAL_ERROR "common directory not found. Check setting of SB_COMMON_DIR (${SB_COMMON_DIR})")
endif()
if(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Unknown architecture. Only cross-compiling for ARM is supported.")
endif()

add_subdirectory(${SBLIB_PATH} sblib_arm)
add_subdirectory(${SBLPC_DIR} CMSIS_CORE_LPC11xx_arm)

# Build options
option(OUT8_BUILD_FLASHSTART "Additionally build all variants for use with the Selfbus bootloader" ON)
set(OUT8_FLASHSTART_ADDRESS "0x3000" CACHE STRING "Application start address when used with the Selfbus bootloader")
# The last flash sector (4 KB) holds the sblib UserEeprom, as configured in the MCUXpresso project (MFlash64 = 0xF000)
set(OUT8_FLASH_END "0xF000" CACHE STRING "End of the flash area usable for code (start of the UserEeprom sector)")
set(OUT8_SW_VERSION "${PROJECT_VERSION}" CACHE STRING "Software version used in the artifact names (keep in sync with APP_VERSION in app_main.cpp)")

include(out8-bcu1.cmake)

# definitions for all variants (as in the MCUXpresso build configurations)
set(OUT8_COMMON_DEFINES
    __USE_CMSIS=CMSIS_CORE_LPC11xx
    __LPC11XX__
    __NEWLIB__
    CORE_M0
    CPP_USE_HEAP
    OUT8
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

set(_common_sources "")
foreach(src ${OUT8_BCU1_COMMON_SRC})
    list(APPEND _common_sources "${SB_COMMON_DIR}/${src}")
endforeach()

set(OUT8_STANDALONE_TARGETS "")
set(OUT8_FLASHSTART_TARGETS "")

# Adds one firmware target.
#   name:        variant name as in the MCUXpresso build configuration, e.g. "o8_4t_bf_ha"
#   flashStart:  "" for a standalone firmware, otherwise application start address (hex)
#   ARGN:        variant specific defines (controller type, BI_STABLE, HAND_ACTUATION, BUSFAIL)
function(out8_add_firmware name flashStart)
    if(flashStart STREQUAL "")
        set(target "${PROJECT_NAME}_${name}")
        set(OUT8_FLASH_ORIGIN "0x0")
        set(OUT8_FLASH_LENGTH "${OUT8_FLASH_END}")
        set(OUT8_RAM_ORIGIN "0x10000000")
        set(OUT8_RAM_LENGTH "0x2000")
    else()
        set(target "${PROJECT_NAME}_Flashstart_${flashStart}_${name}")
        math(EXPR _flashLength "${OUT8_FLASH_END} - ${flashStart}" OUTPUT_FORMAT HEXADECIMAL)
        set(OUT8_FLASH_ORIGIN "${flashStart}")
        set(OUT8_FLASH_LENGTH "${_flashLength}")
        set(OUT8_RAM_ORIGIN "0x100000d4") # bus updater copies 0xd4 bytes in jumpToApplication()
        set(OUT8_RAM_LENGTH "0x1f2c")
    endif()
    set(OUT8_LD_VARIANT "${target}")
    set(linkerScript "${CMAKE_CURRENT_BINARY_DIR}/linkscripts/${target}.ld")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/linkscripts/out8-bcu1.ld.in" "${linkerScript}" @ONLY)

    add_executable(${target} ${OUT8_BCU1_SRC} ${_common_sources})
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${target}_v${OUT8_SW_VERSION}"
        SUFFIX ".axf"
        LINK_DEPENDS "${linkerScript}"
    )
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${SB_COMMON_DIR}/bus_voltage
        ${SB_COMMON_DIR}/hand-actuation
        ${SBLPC_DIR}/inc
        ${SBLIB_PATH}/inc
    )
    target_compile_definitions(${target} PRIVATE ${OUT8_COMMON_DEFINES} ${ARGN})
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-O0>
        $<$<NOT:$<CONFIG:Debug>>:-Os>
    )
    target_link_libraries(${target} PRIVATE CMSIS_CORE_LPC11xx_arm sblib_arm)
    target_link_options(${target} PRIVATE
        # Keep the handlers global with LTO, otherwise DEFINED() in the linker script is false and
        # __valid_user_code_checksum (LPC boot ROM "valid user code" check) is calculated wrong.
        -Wl,--undefined=NMI_Handler
        -Wl,--undefined=HardFault_Handler
        -T "${linkerScript}"
        -Xlinker -Map=$<TARGET_FILE_DIR:${target}>/${target}_v${OUT8_SW_VERSION}.map
    )

    set(_base "$<TARGET_FILE_DIR:${target}>/${target}_v${OUT8_SW_VERSION}")
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> ${_base}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}> ${_base}.bin
        VERBATIM
    )
    if(flashStart STREQUAL "")
        set(OUT8_STANDALONE_TARGETS ${OUT8_STANDALONE_TARGETS} ${target} PARENT_SCOPE)
    else()
        # As in MCUXpresso, bootloader variants are Release only: Debug images do not fit
        # between the bootloader and the UserEeprom sector. Not part of the default "all" target.
        set_target_properties(${target} PROPERTIES EXCLUDE_FROM_ALL TRUE)
        set(OUT8_FLASHSTART_TARGETS ${OUT8_FLASHSTART_TARGETS} ${target} PARENT_SCOPE)
    endif()
endfunction()

# Build variants of the MCUXpresso project (name = configuration name without Debug/Release)
#   o8: out8, 4t/2t: 4MU / 2MU controller, v103: 2MU controller <= v1.03,
#   bf: bus fail detection, ha: hand actuation, bi: bi-stable relays
#   Format: "<name>|<define>,<define>,..."
set(OUT8_VARIANTS
    "o8_4t|TE4"
    "o8_4t_bf|TE4,BUSFAIL"
    "o8_4t_ha|TE4,HAND_ACTUATION"
    "o8_4t_bf_ha|TE4,HAND_ACTUATION,BUSFAIL"
    "o8_4t_bi|TE4,BI_STABLE"
    "o8_4t_ha_bi|TE4,BI_STABLE,HAND_ACTUATION"
    "o8_4t_bf_ha_bi|TE4,BI_STABLE,HAND_ACTUATION,BUSFAIL"
    "o8_2t_v103_bf_ha|MU2_V103,HAND_ACTUATION,BUSFAIL"
)

foreach(variant ${OUT8_VARIANTS})
    string(REPLACE "|" ";" _parts "${variant}")
    string(REPLACE "," ";" _parts "${_parts}")
    list(GET _parts 0 _name)
    list(REMOVE_AT _parts 0)
    out8_add_firmware(${_name} "" ${_parts})
    if(OUT8_BUILD_FLASHSTART)
        out8_add_firmware(${_name} "${OUT8_FLASHSTART_ADDRESS}" ${_parts})
    endif()
endforeach()

# out8-bcu1_all:        standalone firmware, Debug and Release
# out8-bcu1_flashstart: firmware for the Selfbus bootloader, build with configuration Release only
add_custom_target(${PROJECT_NAME}_all DEPENDS ${OUT8_STANDALONE_TARGETS})
if(OUT8_FLASHSTART_TARGETS)
    add_custom_target(${PROJECT_NAME}_flashstart DEPENDS ${OUT8_FLASHSTART_TARGETS})
endif()
message(STATUS "out8-bcu1 standalone targets: ${OUT8_STANDALONE_TARGETS}")
message(STATUS "out8-bcu1 flashstart targets (Release only): ${OUT8_FLASHSTART_TARGETS}")
