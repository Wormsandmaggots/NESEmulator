//
// Created by Radek on 30.12.2024.
//

#include "PPU.h"

namespace ppu {
    constexpr u16 PPUCTRLAddress = 0x2000;
    constexpr u16 PPUMASKAddress = 0x2001;
    constexpr u16 PPUSTATUSAddress = 0x2002;
    constexpr u16 OAMADDRAddress = 0x2003;
    constexpr u16 OAMDATAAddress = 0x2004;
    constexpr u16 PPUSCROLLAddress = 0x2005;
    constexpr u16 PPUADDRAddress = 0x2006;
    constexpr u16 PPUDATAAddress = 0x2007;
    constexpr u16 OAMDMAAddress = 0x2008;
}

using namespace ppu;

PPU::PPU(Memory* shared) {
    this->sharedMemory = shared;
    init(sharedMemory);

    palette = {{
        {0x80, 0x80, 0x80}, {0x00, 0x3D, 0xA6}, {0x00, 0x12, 0xB0}, {0x44, 0x00, 0x96},
        {0xA1, 0x00, 0x5E}, {0xC7, 0x00, 0x28}, {0xBA, 0x06, 0x00}, {0x8C, 0x17, 0x00},
        {0x5C, 0x2F, 0x00}, {0x10, 0x45, 0x00}, {0x05, 0x4A, 0x00}, {0x00, 0x47, 0x2E},
        {0x00, 0x41, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05}, {0x05, 0x05, 0x05},
        {0xC7, 0xC7, 0xC7}, {0x00, 0x77, 0xFF}, {0x21, 0x55, 0xFF}, {0x82, 0x37, 0xFA},
        {0xEB, 0x2F, 0xB5}, {0xFF, 0x29, 0x50}, {0xFF, 0x22, 0x00}, {0xD6, 0x32, 0x00},
        {0xC4, 0x62, 0x00}, {0x35, 0x80, 0x00}, {0x05, 0x8F, 0x00}, {0x00, 0x8A, 0x55},
        {0x00, 0x99, 0xCC}, {0x21, 0x21, 0x21}, {0x09, 0x09, 0x09}, {0x09, 0x09, 0x09},
        {0xFF, 0xFF, 0xFF}, {0x0F, 0xD7, 0xFF}, {0x69, 0xA2, 0xFF}, {0xD4, 0x80, 0xFF},
        {0xFF, 0x45, 0xF3}, {0xFF, 0x61, 0x8B}, {0xFF, 0x88, 0x33}, {0xFF, 0x9C, 0x12},
        {0xFA, 0xBC, 0x20}, {0x9F, 0xE3, 0x0E}, {0x2B, 0xF0, 0x35}, {0x0C, 0xF0, 0xA4},
        {0x05, 0xFB, 0xFF}, {0x5E, 0x5E, 0x5E}, {0x0D, 0x0D, 0x0D}, {0x0D, 0x0D, 0x0D},
        {0xFF, 0xFF, 0xFF}, {0xA6, 0xFC, 0xFF}, {0xB3, 0xEC, 0xFF}, {0xDA, 0xAB, 0xEB},
        {0xFF, 0xA8, 0xF9}, {0xFF, 0xAB, 0xB3}, {0xFF, 0xD2, 0xB0}, {0xFF, 0xEF, 0xA6},
        {0xFF, 0xF7, 0x9C}, {0xD7, 0xE8, 0x95}, {0xA6, 0xED, 0xAF}, {0xA2, 0xF2, 0xDA},
        {0x99, 0xFF, 0xFC}, {0xDD, 0xDD, 0xDD}, {0x11, 0x11, 0x11}, {0x11, 0x11, 0x11}
    }};
}

void PPU::init(Memory* shared) {
    frameReady = false;
    scanline = 0;

    regs.T = 0;
    regs.V = 0;
    regs.W = 0;
    regs.X = 0;

    regs.OAMAddr = &shared->getReference(OAMADDRAddress);
    regs.OAMData = &shared->getReference(OAMDATAAddress);
    regs.PPUAddr = &shared->getReference(PPUADDRAddress);
    regs.PPUControl = &shared->getReference(PPUCTRLAddress);
    regs.PPUData = &shared->getReference(PPUDATAAddress);
    regs.PPUMask = &shared->getReference(PPUMASKAddress);
    regs.PPUScroll = &shared->getReference(PPUSCROLLAddress);
    regs.PPUStatus = &shared->getReference(PPUSTATUSAddress);

    cycle = 0;

    frameBuffer.resize(resolution.x * resolution.y);

    oam.init();
    vram.init();
}

void PPU::update() {
    // // Symulacja renderowania wszystkich widocznych linii
    // for (int y = 0; y < resolution.y; ++y) {
    //     for (int x = 0; x < resolution.x; ++x) {
    //         renderPixel(x, y);
    //     }
    // }

    for (int scanline = 0; scanline < resolution.y; ++scanline) {
        renderBackgroundLine(scanline);
    }

    // Po zakończeniu ramki oznacz ją jako gotową
    frameReady = true;
}

std::vector<Color> PPU::getFrameBuffer() const {
    return frameBuffer;
}

void PPU::renderPixel(u16 x, u16 y) {
    if (x < 0 || x >= resolution.x || y < 0 || y >= resolution.y) {
        return; // Ignoruj piksele poza ekranem
    }

    // Oblicz pozycję kafelka (8x8)
    int tileX = x / 8;
    int tileY = y / 8;

    // Offset pikseli w kafelku
    int pixelX = x % 8;
    int pixelY = y % 8;

    // Oblicz adres kafelka w pamięci
    u16 tileIndex = (tileY * 32 + tileX) & 0x3FF; // Tabela kafelków mieści się w 10 bitach
    u16 tileAddress = PPUCTRLAddress + tileIndex;

    // Pobierz dane kafelka
    u8 tileLow = sharedMemory->getReference(tileAddress + pixelY);
    u8 tileHigh = sharedMemory->getReference(tileAddress + pixelY + 8);

    // Oblicz kolor piksela
    u8 paletteIndex = ((tileHigh >> (7 - pixelX)) & 1) << 1 | ((tileLow >> (7 - pixelX)) & 1);
    Color c = palette[paletteIndex];

    // Zapisz kolor do bufora klatki
    frameBuffer[y * resolution.x + x] = c;
}

void PPU::renderBackgroundLine(int scanline) {
    if (scanline < 0 || scanline >= resolution.y) {
        return; // Ignoruj linie poza widocznym ekranem
    }

    for (int x = 0; x < resolution.x; ++x) {
        // Oblicz pozycję w Name Table
        int tileX = (x + regs.X) / 8;
        int tileY = (scanline + regs.getVerticalScroll()) / 8;

        u16 nameTableBase = 0x2000 + ((*regs.PPUControl & 0b11) * 0x400); // Wybranie właściwej Name Table
        u16 tileIndex = (tileY % 30) * 32 + (tileX % 32); // Indeks kafelka w Name Table
        u8 tileID = sharedMemory->getReference(nameTableBase + tileIndex);

        // Oblicz adres w Pattern Table
        u16 patternTableBase = (*regs.PPUControl & 0b0001) ? 0x1000 : 0x0000; // Wybranie Pattern Table
        u16 tileAddress = patternTableBase + tileID * 16;

        // Wczytaj dane kafelka
        int pixelY = (scanline + regs.getVerticalScroll()) % 8; // Wiersz w kafelku
        u8 tileLow = sharedMemory->getReference(tileAddress + pixelY);
        u8 tileHigh = sharedMemory->getReference(tileAddress + pixelY + 8);

        // Oblicz kolor piksela
        int pixelX = (x + regs.X) % 8; // Kolumna w kafelku
        u8 paletteIndex = ((tileHigh >> (7 - pixelX)) & 1) << 1 | ((tileLow >> (7 - pixelX)) & 1);

        // Pobierz informację o atrybucie (palecie)
        int attributeX = tileX / 4;
        int attributeY = tileY / 4;
        u16 attributeAddress = nameTableBase + 0x03C0 + (attributeY * 8 + attributeX);
        u8 attributeByte = sharedMemory->getReference(attributeAddress);

        // Przesunięcie w bajcie atrybutów
        int shift = ((tileY % 4 >= 2) ? 4 : 0) + ((tileX % 4 >= 2) ? 2 : 0);
        u8 paletteSelect = (attributeByte >> shift) & 0b11;

        // Pobierz kolor z palety
        u8 colorIndex = paletteSelect * 4 + paletteIndex;
        Color c = palette[colorIndex]; // Adres palety

        // Zapisz kolor do bufora
        frameBuffer[scanline * resolution.x + x] = c;
    }
}
