/**
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2026
 * @par License
 * GNU General Public License version 3.
 */
#ifndef OUT8_TEST_MODEL_H_
#define OUT8_TEST_MODEL_H_
#include <cstdint>
#include <stdexcept>
namespace model
{
    struct Config
    {
        uint32_t divider16 = 16;
        uint32_t wrap = 65535;
        bool invertA = false;
        bool invertB = false;
    };
    struct Slice
    {
        Config config;
        uint32_t level[2] = {};
        bool enabled = false;
        uint32_t initializations = 0;
    };
    inline Slice slices[12];
    inline bool pins[48] = {};
    inline int function[48] = {};
    inline bool direction[48] = {};
    inline uint32_t now = 0;
    inline uint32_t clock = 150000000;
    inline uint32_t delayed = 0;
} // namespace model
#endif
