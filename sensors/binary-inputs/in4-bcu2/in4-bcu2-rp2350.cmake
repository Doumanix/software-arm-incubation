# in4-bcu2 for the RP2350 (sblib RP2350 port), included by CMakeLists.txt for SBLIB_MCU=RP2350.
#
# Same sources as the LPC1115 build, platform differences only in src/config.h (input and LED pins
# per sblib board). Uses the sblib main(), the Pico SDK startup and linker script and the sblib
# storage area at the end of the flash for the BCU2 UserEeprom.

# Path to the sblib repository. Relative paths are relative to this directory.
set(SB_PATH_TO_SBLIB_REPO "../../../software-arm-lib" CACHE PATH "Path to the sblib repository (e.g. /home/user/sblib).")
if(IS_ABSOLUTE "${SB_PATH_TO_SBLIB_REPO}")
    set(_sblib_repo "${SB_PATH_TO_SBLIB_REPO}")
else()
    set(_sblib_repo "${CMAKE_CURRENT_SOURCE_DIR}/${SB_PATH_TO_SBLIB_REPO}")
endif()
set(SBLIB_PATH "${_sblib_repo}/sblib" CACHE PATH "Path to the sblib")
if(NOT EXISTS "${SBLIB_PATH}")
    message(FATAL_ERROR "sblib project not found. Check setting of SB_PATH_TO_SBLIB_REPO/SBLIB_PATH (${SBLIB_PATH})")
endif()

include(${_sblib_repo}/cmake/rp2350/pico_sdk_setup.cmake)

project(in4-bcu2 LANGUAGES C CXX ASM VERSION 0.02)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_subdirectory(${SBLIB_PATH} sblib)

set(IN4_SW_VERSION "${PROJECT_VERSION}" CACHE STRING "Software version used in the artifact names (keep in sync with APP_VERSION in app_main.cpp)")

include(${CMAKE_CURRENT_LIST_DIR}/in4-bcu2.cmake)

# LPC startup code is replaced by the Pico SDK runtime
list(REMOVE_ITEM IN4_BCU2_SRC src/cr_startup_lpc11xx.cpp)

# The Pico SDK toolchain does not set CMAKE_SIZE; look for size next to the compiler.
if(NOT CMAKE_SIZE)
    get_filename_component(_toolchain_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_program(CMAKE_SIZE NAMES arm-none-eabi-size HINTS "${_toolchain_bin}" NO_DEFAULT_PATH REQUIRED)
endif()

# Variant per sblib board (input and LED pins in src/config.h)
#   pico2tb: Pico 2 test board
if(SBLIB_BOARD STREQUAL "pico2_testboard")
    set(target "${PROJECT_NAME}_pico2tb")
else()
    message(FATAL_ERROR "in4-bcu2: no variant for SBLIB_BOARD=${SBLIB_BOARD} (add the input pins in src/config.h)")
endif()

add_executable(${target} ${IN4_BCU2_SRC})
set_target_properties(${target} PROPERTIES OUTPUT_NAME "${target}_v${IN4_SW_VERSION}")
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${target} PRIVATE pico_stdlib sblib_rp2350)
target_link_options(${target} PRIVATE "-Wl,-Map=$<TARGET_FILE_DIR:${target}>/${target}_v${IN4_SW_VERSION}.map")
pico_enable_stdio_uart(${target} 0)
pico_enable_stdio_usb(${target} 0)

set(_base "$<TARGET_FILE_DIR:${target}>/${target}_v${IN4_SW_VERSION}")
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> ${_base}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}> ${_base}.bin
    VERBATIM
)
add_custom_target(${PROJECT_NAME}_all DEPENDS ${target})
message(STATUS "in4-bcu2 RP2350 target: ${target}")
