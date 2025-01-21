//
// Created by Radek on 30.12.2024.
//

#ifndef PPU_H
#define PPU_H
#include <array>

#include "Cartridge.h"
#include "Memory.h"
#include "Types.h"
#include "Settings.h"
#include "NESHelpers.h"

class PPU {
public:
    explicit PPU(Memory* shared);
    ~PPU() = default;

    void init(Memory* shared);

    void step(nes_cycle_t count);

    void step_ppu(nes_ppu_cycle_t count);

    std::vector<u8> getFrameBuffer() const;
    std::vector<u32> getFrame() const;

    u8 readVram(u16 addr);
    void writeVram(u16 addr, u8 value);
    void writeVram(u16 addr, u8 *src, u16 src_size);

    u8* getVRam();

    static std::function<void(u16)> setOAMDMA;

    void setMirroring(nes_mapper_flags);
private:
    ppu::Registers regs{};

    Memory* sharedMemory = null;

    //Memory oam = Memory(oamSize);
    std::vector<u8> vram;
    std::array<Color, paletteSize> palette;

    u16 scanline = 0;

    u8 tileIndex = 0;
    u8 tilePaletteBit32 = 0;
    u8 bitPlane0;
    u8 _pixel_cycle[8];
    u8 xOffset = 0;
    u8 lastSpriteID = 0;
    u8 hasSprite0 = 0;
    u8 lastSpriteY = 0;
    u32 frameCount = 0;

    nes_cycle_t _master_cycle;
    nes_ppu_cycle_t _scanline_cycle;

    nes_mapper_flags mirroring;

    u8* entireFrameBuffer;
    std::vector<u8> frameBuffer1;
    std::vector<u8> frameBuffer2;
    std::vector<u8> frameBufferBG;

    std::vector<ppu::Sprite> spriteBuffer;

    bool frameReady = false;

    void tilesPipeline();
    void spritesPipeline();

    void fetchTile();
    void fetch_sprite(uint8_t sprite_id);

    u8 read_pattern_table_column(bool sprite, u8 tile_index, u8 bitplane, u8 tile_row_index);
    u8 read_pattern_table_column_8x16_sprite(uint8_t tile_index, uint8_t bitplane, uint8_t tile_row_index);
    u8 get_palette_color(bool is_background, uint8_t palette_index_4_bit);

    ppu::Sprite *getSprite(u8 sprite_id) const;

    void swap_buffer();

    bool is_ready() const { return _master_cycle > nes_ppu_cycle_t(29658); }

    void redirectAddress(u16& address);

    //https://www.nesdev.org/wiki/PPU_registers

    //PPUMASK 0x2001
    // bool showBackground() const;
    // bool showBackgroundLeft() const;
    // bool showSprites() const;
    // bool showSpritesLeft() const;
};



#endif //PPU_H
