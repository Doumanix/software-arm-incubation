cmake_minimum_required(VERSION 3.28)

# Target microcontroller. RP2350: separate build in in4-bcu2-rp2350.cmake (Pico SDK).
set(SBLIB_MCU "LPC1115" CACHE STRING "Target microcontroller (LPC1115, RP2350)")
set_property(CACHE SBLIB_MCU PROPERTY STRINGS LPC1115 RP2350)
if(SBLIB_MCU STREQUAL "RP2350")
    include(${CMAKE_CURRENT_LIST_DIR}/in4-bcu2-rp2350.cmake)
    return()
endif()

project(in4-bcu2 LANGUAGES C CXX VERSION 0.02)

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
if(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Unknown architecture. Only cross-compiling for ARM is supported.")
endif()

add_subdirectory(${SBLIB_PATH} sblib_arm)
add_subdirectory(${SBLPC_DIR} CMSIS_CORE_LPC11xx_arm)

# The last flash sector (4 KB) holds the sblib UserEeprom, as configured in the MCUXpresso project (MFlash32 = 0xF000)
set(IN4_FLASH_END "0xF000" CACHE STRING "End of the flash area usable for code (start of the UserEeprom sector)")
set(IN4_SW_VERSION "${PROJECT_VERSION}" CACHE STRING "Software version used in the artifact names (keep in sync with APP_VERSION in app_main.cpp)")

include(in4-bcu2.cmake)

# Firmware as the Debug and Release configurations of the MCUXpresso project
set(target "${PROJECT_NAME}")
set(IN4_LD_VARIANT "${target}")
set(IN4_FLASH_ORIGIN "0x0")
set(IN4_FLASH_LENGTH "${IN4_FLASH_END}")
set(IN4_RAM_ORIGIN "0x10000000")
set(IN4_RAM_LENGTH "0x2000")
set(linkerScript "${CMAKE_CURRENT_BINARY_DIR}/linkscripts/${target}.ld")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/linkscripts/in4-bcu2.ld.in" "${linkerScript}" @ONLY)

add_executable(${target} ${IN4_BCU2_SRC})
set_target_properties(${target} PROPERTIES
    OUTPUT_NAME "${target}_v${IN4_SW_VERSION}"
    SUFFIX ".axf"
    LINK_DEPENDS "${linkerScript}"
)
target_include_directories(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SBLPC_DIR}/inc
    ${SBLIB_PATH}/inc
)
target_compile_definitions(${target} PRIVATE
    __USE_CMSIS=CMSIS_CORE_LPC11xx
    __LPC11XX__
    __NEWLIB__
    CORE_M0
    CPP_USE_HEAP
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)
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
    -Xlinker -Map=$<TARGET_FILE_DIR:${target}>/${target}_v${IN4_SW_VERSION}.map
)

set(_base "$<TARGET_FILE_DIR:${target}>/${target}_v${IN4_SW_VERSION}")
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> ${_base}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}> ${_base}.bin
    VERBATIM
)
add_custom_target(${PROJECT_NAME}_all DEPENDS ${target})
