/* Copyright (C) 2026 Doumanix <doumanix@gmx.de>. GNU GPL version 3. */
#ifndef OUT8_TEST_MODEL_H_
#define OUT8_TEST_MODEL_H_
#include <cstdint>
#include <stdexcept>
namespace model
{
    struct Config
    {
        unsigned int divider16 = 16;
        unsigned int wrap = 65535;
        bool invertA = false;
        bool invertB = false;
    };
    struct Slice
    {
        Config config;
        unsigned int level[2] = {};
        bool enabled = false;
        unsigned int initializations = 0;
    };
    inline Slice slices[12];
    inline bool pins[48] = {};
    inline int function[48] = {};
    inline bool direction[48] = {};
    inline unsigned int now = 0;
    inline unsigned int clock = 150000000;
    inline unsigned int delayed = 0;
} // namespace model
#endif
