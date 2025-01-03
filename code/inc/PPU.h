//
// Created by Radek on 30.12.2024.
//

#ifndef PPU_H
#define PPU_H
#include <array>

#include "Memory.h"
#include "Types.h"
#include "Settings.h"
#include "NESHelpers.h"

class PPU {
public:
    explicit PPU(Memory* shared);
    ~PPU() = default;

    void init(Memory* shared);

    void update();

    std::vector<Color> getFrameBuffer() const;

private:
    ppu::Registers regs{};

    Memory* sharedMemory = null;

    Memory vram = Memory(vRamSize);
    Memory oam = Memory(oamSize);
    std::array<Color, paletteSize> palette;

    u16 scanline = 0;
    u16 cycle = 0;

    std::vector<Color> frameBuffer;
    bool frameReady = false;

    void renderPixel(u16 x, u16 y);
    void renderBackgroundLine(int scanline);

    //https://www.nesdev.org/wiki/PPU_registers

    //PPUMASK 0x2001
    // bool showBackground() const;
    // bool showBackgroundLeft() const;
    // bool showSprites() const;
    // bool showSpritesLeft() const;
};



#endif //PPU_H
