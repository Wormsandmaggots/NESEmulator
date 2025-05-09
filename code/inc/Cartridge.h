//
// Created by Radek on 01.11.2024.
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <vector>

#include "Types.h"

class PPU;
class Memory;

enum nes_mapper_flags : u8
{
    nes_mapper_flags_none = 0,

    nes_mapper_flags_mirroring_mask = 0x3,

    // A, B
    // A, B
    nes_mapper_flags_vertical_mirroring = 0x2,

    // A, A
    // B, B
    nes_mapper_flags_horizontal_mirroring = 0x3,
};

struct FileHeader
{
    const char magic[4]; // "NES" + 0x1A
    u8 prg_rom_size;     // amount of 16KB PRG_ROM banks
    u8 chr_rom_size;     // amount of 8KB CHR_ROM banks
    u8 flag6;            // Flags for mirroring, trainer, mapper
    u8 flag7;            // Flags for mapper, NES 2.0
    u8 prg_ram_size;     // Size of PRG_RAM
    u8 flag9;            // Region flag(NTSC/PAL)
    u8 flag10;           // Additional extensions
    u8 padding[5];       // Reserved bytes (should be 0)
};

struct NESFile {
    std::vector<u8> prg_rom;
    std::vector<u8> chr_rom;
    bool has_trainer;
    u8 mapper;
    u8 mirroring;
};

class Cartridge {
public:
    explicit Cartridge(const char* path);
    ~Cartridge();
    bool load();

    const NESFile* getNESFile() const;
    void loadToMemory(Memory* mem);
    void loadToVRam(PPU* ppu) const;

    nes_mapper_flags getMirroring() const;

private:
    NESFile* nesFile = null;
    const char* path = null;
};

#endif //CARTRIDGE_H