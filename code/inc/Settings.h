//
// Created by Radek on 01.11.2024.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include <SDL_rect.h>

#include "Types.h"

constexpr u32 ramSize = 0x10000; //65536 or 64KB
constexpr u8 eof_value = 0x1A;
constexpr const char* NES = "NES\x1A";
constexpr i8 illegalCycles = -1;
constexpr u16 vRamSize = 0x4000;
constexpr u16 oamSize = 0x100;
constexpr u8 paletteSize = 64;
constexpr vec2d resolution{256, 240};

#endif //SETTINGS_H
