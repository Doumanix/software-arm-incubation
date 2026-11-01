cmake_minimum_required(VERSION 3.28)

# Host unit tests of the out8-bcu1 application logic (Catch2), the CMake counterpart of the
# MCUXpresso project in this directory (.cproject/.project). Modelled on the host test projects
# of the sblib (test/lib-test-cases). The application is compiled from
# actuators/outputs/out8-bcu1 without change, as linked source folder in the MCUXpresso project.
project(out8-bcu1-test LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose output from the build system")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Creates compile_commands.json file for easier debugging and IDE support

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to the sblib repository, as in the CMakeLists.txt of the application
set(SB_PATH_TO_SBLIB_REPO "../../../../software-arm-lib" CACHE PATH "Path to the sblib repository (e.g. /home/user/sblib)")
if(IS_ABSOLUTE "${SB_PATH_TO_SBLIB_REPO}")
    set(_sblib_repo "${SB_PATH_TO_SBLIB_REPO}")
else()
    set(_sblib_repo "${CMAKE_CURRENT_SOURCE_DIR}/${SB_PATH_TO_SBLIB_REPO}")
endif()
get_filename_component(SBLIB_REPO "${_sblib_repo}" ABSOLUTE)

set(CATCH_PATH "${SBLIB_REPO}/Catch" CACHE PATH "Path to the Catch project")
set(SBLIB_PATH "${SBLIB_REPO}/sblib" CACHE PATH "Path to the sblib project")
set(SBLIB_TEST_PATH "${SBLIB_REPO}/test/sblib" CACHE PATH "Path to the sblib-test project")
set(OUT8_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../actuators/outputs/out8-bcu1" CACHE PATH "Path to the out8-bcu1 application")
set(SB_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../common" CACHE PATH "Path to the incubation 'common' directory")

foreach(_required_dir
        "${CATCH_PATH}"
        "${SBLIB_PATH}"
        "${SBLIB_TEST_PATH}"
        "${OUT8_APP_DIR}/src"
        "${OUT8_APP_DIR}/inc"
        "${SB_COMMON_DIR}/hand-actuation")
    if(NOT EXISTS "${_required_dir}")
        message(FATAL_ERROR "Directory not found: ${_required_dir}. Check SB_PATH_TO_SBLIB_REPO (${SBLIB_REPO}).")
    endif()
endforeach()

# Architecture of the host test. The MCUXpresso project builds -m32, the sblib host tests are
# built for x86 and x64; x64 is the architecture used by the CI.
set(SBLIB_ARCHITECTURE "x64" CACHE STRING "Architecture to build the host test for (x64, x86)")
set_property(CACHE SBLIB_ARCHITECTURE PROPERTY STRINGS x64 x86)
if(NOT SBLIB_ARCHITECTURE MATCHES "^(x64|x86)$")
    message(FATAL_ERROR "Unknown SBLIB_ARCHITECTURE '${SBLIB_ARCHITECTURE}'. Valid values: x64, x86")
endif()

set(TARGET ${PROJECT_NAME}_${SBLIB_ARCHITECTURE})

# Enable testing
include(CTest)

include(${CATCH_PATH}/catch.cmake)               # SBLIB_CATCH_SRC (catch.hpp)
add_subdirectory(${SBLIB_TEST_PATH} sblib-test)  # sblib-test_<arch>: cpu-emu, protocol.cpp, and sblib_<arch>

# Test sources of this directory
set(OUT8_TEST_SRC
    src/out8.h
    src/fixture.cpp
    src/app_fixture.cpp
    src/main.cpp
    src/out8.cpp
    src/simple.cpp
    src/special.cpp
)

# Application sources, as in the MCUXpresso project: all sources of the application except the
# LPC start-up code (src/cr_startup_lpc11xx.cpp), plus the linked folder common/hand-actuation
set(OUT8_APP_SRC
    ${OUT8_APP_DIR}/src/app_main.cpp
    ${OUT8_APP_DIR}/src/outputs.cpp
    ${OUT8_APP_DIR}/src/outputsBiStable.cpp
    ${SB_COMMON_DIR}/hand-actuation/hand_actuation.cpp
)

add_executable(${TARGET}
    ${SBLIB_CATCH_SRC}
    ${OUT8_TEST_SRC}
    ${OUT8_APP_SRC}
)

# The include directories of sblib, sblib-test, cpu-emu and Catch come with sblib-test_<arch>
target_include_directories(${TARGET} PRIVATE
    ${OUT8_APP_DIR}/inc
    ${OUT8_APP_DIR}/src
    ${SB_COMMON_DIR}/hand-actuation
)

# Build variables of the MCUXpresso project: num_outputs=OUT8, no BI_STABLE, HAND_ACTUATION or BUSFAIL
target_compile_definitions(${TARGET} PRIVATE
    OUT8
    BCU_TYPE=10
    __LPC11XX__
    IAP_EMULATION
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_compile_options(${TARGET} PRIVATE
    -Wall -Wlogical-op -Woverloaded-virtual -Wextra
    -fmessage-length=0
    -g3
    $<$<CONFIG:Debug>:-O0>
    $<$<NOT:$<CONFIG:Debug>>:-Os>
    $<$<STREQUAL:${SBLIB_ARCHITECTURE},x86>:-m32>
    $<$<STREQUAL:${SBLIB_ARCHITECTURE},x64>:-m64>
)

target_link_options(${TARGET} PRIVATE
    $<$<NOT:$<CONFIG:Debug>>:-Os>
    $<$<STREQUAL:${SBLIB_ARCHITECTURE},x86>:-m32>
    $<$<STREQUAL:${SBLIB_ARCHITECTURE},x64>:-m64>
)

target_link_libraries(${TARGET} PRIVATE sblib-test_${SBLIB_ARCHITECTURE})

# Every application case is now checked directly, including Logic.
set(OUT8_TEST_CASES
    "OUT8 - Locking"
    "OUT8 - Logic"
    "OUT8 - Simple"
    "OUT8 - Simple Inverted"
    "OUT8 - Simple Delay"
    "OUT8 - Simple Timed"
)
foreach(_test_case IN LISTS OUT8_TEST_CASES)
    string(REPLACE " " "_" _test_name "${_test_case}")
    add_test(NAME ${TARGET}_${_test_name}
        COMMAND ${TARGET} "${_test_case}" --order decl --reporter junit
                --out $<TARGET_FILE_DIR:${TARGET}>/${_test_name}_result.xml
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${TARGET}>)
endforeach()
add_test(NAME out8_complete_suite
    COMMAND ${TARGET} --order decl --reporter junit
            --out $<TARGET_FILE_DIR:${TARGET}>/complete_result.xml
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${TARGET}>)
