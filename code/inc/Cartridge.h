//
// Created by Radek on 01.11.2024.
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <vector>

#include "Types.h"

struct FileHeader
{
    const char magic[4];         // "NES" + 0x1A
    u8 prg_rom_size;     // Liczba 16 KB banków PRG-ROM
    u8 chr_rom_size;     // Liczba 8 KB banków CHR-ROM
    u8 flag6;            // Flagi dla mirroring, trenera, mappera
    u8 flag7;            // Flagi dla mappera, NES 2.0
    u8 prg_ram_size;     // Rozmiar PRG-RAM (w bankach 8 KB)
    u8 flag9;            // Flaga regionu (NTSC/PAL)
    u8 flag10;           // Dodatkowe rozszerzenia
    u8 padding[5];       // Zarezerwowane bajty (powinny być zerami)
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
    bool load(const char* path);

private:
    NESFile* nesFile = null;
    const char* path = null;
};



#endif //CARTRIDGE_H
