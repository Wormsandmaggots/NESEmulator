//
// Created by Radek on 01.11.2024.
//

#include "Cartridge.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>

#include "Logger.h"
#include "LogMessages.h"
#include "Settings.h"
#include "Utils.h"
#include "Memory.h"
#include "NESHelpers.h"
#include "PPU.h"

bool Cartridge::load() {
    nesFile = new NESFile;
    FileHeader header{};

    this->path = path;

    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path, std::ifstream::in | std::ifstream::binary);
    if (!file) {
        ERRORLOG(error::cantOpenFile);
        return false;
    }

    assert(sizeof(header) == 16);

    file.read((char*)&header, sizeof(header));

    if (strncmp(header.magic, NES, 4) != 0) {
        ERRORLOG(error::unsupportedFileFormat);
        return false;
    }

    nesFile->has_trainer = header.flag6 & Bit2;

    nesFile->mapper = (header.flag6 >> 4) | (header.flag7 & 0xF0);

    nesFile->mirroring = header.flag6 & Bit0;

    if (nesFile->has_trainer) {
        file.seekg(512, std::ios::cur);
    }

    int prg_rom_size = header.prg_rom_size * 0x4000;    // 16KB
    int chr_rom_size = header.chr_rom_size * 0x2000;    // 8KB

    nesFile->prg_rom = std::vector<uint8_t>(prg_rom_size);
    nesFile->chr_rom = std::vector<uint8_t>(chr_rom_size);

    file.read((char *)nesFile->prg_rom.data(), nesFile->prg_rom.size());
    file.read((char *)nesFile->chr_rom.data(), nesFile->chr_rom.size());

    return true;
}

const NESFile* Cartridge::getNESFile() const {
    return nesFile;
}

void Cartridge::loadToMemory(Memory* mem) const {
    switch(nesFile->mapper) {
        case 0: { //mapper0
            mem->write(0x8000, nesFile->prg_rom.data(), nesFile->prg_rom.size());

            if(nesFile->prg_rom.size() == 0x4000) {
                mem->write(0xC000, nesFile->prg_rom.data(), nesFile->prg_rom.size());
            }

            break;
        }
        default:
            ERRORLOG(error::unsupportedMapper);
    }
}

void Cartridge::loadToVRam(PPU *ppu) const {
    ppu->setMirroring(getMirroring());
    ppu->writeVram(0x0, nesFile->chr_rom.data(), nesFile->chr_rom.size());
}

nes_mapper_flags Cartridge::getMirroring() const {
    nes_mapper_flags flags = nes_mapper_flags_none;

    if(nesFile->mirroring)
        flags = nes_mapper_flags(flags | nes_mapper_flags_vertical_mirroring);
    else
        flags = nes_mapper_flags(flags | nes_mapper_flags_horizontal_mirroring);

    return flags;
}


Cartridge::Cartridge(const char *path) {
    this->path = path;
}

Cartridge::~Cartridge() {
    safeDelete(nesFile);
}
