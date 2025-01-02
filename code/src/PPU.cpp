//
// Created by Radek on 30.12.2024.
//

#include "PPU.h"

namespace ppu {
    constexpr u16 PPUMASKAddress = 0x2001;
}

using namespace ppu;

PPU::PPU(Memory *shared) {
    sharedMemory = shared;
}

void PPU::init() {
    frameReady = false;
    scanline = 0;
    regs.T = 0;
    regs.V = 0;
    regs.W = 0;
    regs.X = 0;
    cycle = 0;

    frameBuffer.resize(resolution.x * resolution.y, 0);
}

void PPU::update() {
    if(scanline < 240 && cycle < 256)
        renderPixel();

    cycle++;

    if(cycle >= 341) {
        cycle = 0;
        scanline++;
        if(scanline > 261) {
            scanline = 0;
            frameReady = true;
        }
    }
}

void PPU::renderPixel() {
    // Oblicz pozycję piksela na ekranie
    int x = cycle - 1;  // Pierwszy cykl zaczyna się od 1
    int y = scanline;

    // Sprawdź, czy piksel jest na ekranie (240x256)
    if (x < 0 || x >= 256 || y < 0 || y >= 240) {
        return;
    }

    // Tło: odczytanie wartości bitów dla bieżącego piksela
    uint8_t background_pixel = 0;
    uint8_t background_palette = 0;

    if (showBackground() && (showBackgroundLeft() || x >= 8)) {
        uint16_t bit_offset = 15 - regs.X;  // Offset dla bieżącego piksela w shifcie
        uint8_t low_bit = (bgShiftPatternLow >> bit_offset) & 1;
        uint8_t high_bit = (bgShiftPatternHigh >> bit_offset) & 1;

        background_pixel = (high_bit << 1) | low_bit;

        uint8_t palette_low = (bgShiftAttribLow >> bit_offset) & 1;
        uint8_t palette_high = (bgShiftAttribHigh >> bit_offset) & 1;
        background_palette = (palette_high << 1) | palette_low;
    }

    // Sprite'y: odczytanie wartości bitów dla bieżącego piksela
    uint8_t sprite_pixel = 0;
    uint8_t sprite_palette = 0;
    bool sprite_priority = false;

    if (showSprites() && (showSpritesLeft() || x >= 8)) {
        for (int i = 0; i < 8; i++) {  // NES obsługuje max 8 sprite'ów na linię
            int sprite_x = spriteBuffer[i].x;
            int sprite_y = spriteBuffer[i].y;
            if (sprite_x <= x && (x - sprite_x) < 8) {
                int pixel_offset = x - sprite_x;

                uint8_t low_bit = (spriteBuffer[i].patternLow >> (7 - pixel_offset)) & 1;
                uint8_t high_bit = (spriteBuffer[i].patternHigh >> (7 - pixel_offset)) & 1;
                sprite_pixel = (high_bit << 1) | low_bit;

                if (sprite_pixel != 0) {  // Tylko widoczne piksele sprite'a
                    sprite_palette = spriteBuffer[i].attributes & 0x03;
                    sprite_priority = !(spriteBuffer[i].attributes & 0x20);  // Bit 5: priorytet
                    break;
                }
            }
        }
    }

    // Wybór końcowego koloru piksela na podstawie priorytetu sprite'ów
    uint8_t final_palette = 0;
    uint8_t final_pixel = 0;

    if (background_pixel == 0 && sprite_pixel == 0) {
        final_pixel = 0;  // Domyślny kolor (czarny)
    } else if (background_pixel == 0) {
        final_palette = sprite_palette + 0x10;
        final_pixel = sprite_pixel;
    } else if (sprite_pixel == 0) {
        final_palette = background_palette;
        final_pixel = background_pixel;
    } else {
        if (sprite_priority) {
            final_palette = sprite_palette + 0x10;
            final_pixel = sprite_pixel;
        } else {
            final_palette = background_palette;
            final_pixel = background_pixel;
        }
    }

    // Odczytanie koloru z palety
    uint8_t color_index = readVRAM(0x3F00 + (final_palette << 2) + final_pixel);
    uint32_t rgb_color = nesPalette[color_index & 0x3F];  // Mapowanie do 64 kolorów NES

    // Zapisanie piksela do bufora ramki
    frameBuffer[y * 256 + x] = rgb_color;
}

using enum Bit;

bool PPU::showBackground() const {
    return (*sharedMemory)[PPUMASKAddress] & Bit5;
}

bool PPU::showBackgroundLeft() const {
    return (*sharedMemory)[PPUMASKAddress] & Bit3;
}

bool PPU::showSprites() const {
    return (*sharedMemory)[PPUMASKAddress] & Bit6;
}

bool PPU::showSpritesLeft() const {
    return (*sharedMemory)[PPUMASKAddress] & Bit4;
}
