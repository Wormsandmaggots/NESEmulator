//
// Created by Radek on 01.11.2024.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include <SDL_rect.h>

#include "Types.h"

constexpr const u32 ramSize = 0x10000; //65536 or 64KB
constexpr const u8 eof_value = 0x1A;
constexpr const char* NES = "NES\x1A";
constexpr const i8 illegalCycles = -1;
constexpr const u16 vRamSize = 0x800;
constexpr const u16 oamSize = 0x100;
constexpr const u8 paletteSize = 64;
constexpr const vec2d resolution{256, 240};

#endif //SETTINGS_H
