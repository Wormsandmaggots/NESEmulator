//
// Created by Radek on 30.12.2024.
//

#ifndef PPU_H
#define PPU_H
#include "Memory.h"
#include "Types.h"
#include "Settings.h"
#include "NESHelpers.h"

class Memory;

class PPU {
public:
    explicit PPU(Memory* shared);
    ~PPU() = default;

    void init();

    void update();

private:
    ppu::Registers regs{};

    Memory* sharedMemory = null;

    Memory vram = Memory(vRamSize);
    Memory oam = Memory(oamSize);
    Memory palette = Memory(paletteSize);

    u16 scanline = 0;
    u16 cycle = 0;

    std::vector<u32> frameBuffer;
    bool frameReady = false;

    void renderPixel();

    bool showBackground() const;
    bool showBackgroundLeft() const;
    bool showSprites() const;
    bool showSpritesLeft() const;
};



#endif //PPU_H
