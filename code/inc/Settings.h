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
constexpr const u16 oamSize = 256;
constexpr const u8 paletteSize = 32;
constexpr const vec2d resolution{256, 240};
constexpr const u16* PPURegistersAddresses = new u16[]{0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007};

#endif //SETTINGS_H
