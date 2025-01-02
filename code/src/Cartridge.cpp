//
// Created by Radek on 01.11.2024.
//

#include "Cartridge.h"

#include <cstring>
#include <fstream>
#include <vector>

#include "Logger.h"
#include "LogMessages.h"
#include "Settings.h"
#include "Utils.h"
#include "Memory.h"

bool Cartridge::load() {
    nesFile = new NESFile;
    FileHeader header{};

    this->path = path;

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        ERRORLOG(error::cantOpenFile);
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));

    if (strncmp(header.magic, NES, 4) != 0) {
        ERRORLOG(error::unsupportedFileFormat);
        return false;
    }

    nesFile->has_trainer = header.flag6 & 0x04;

    nesFile->mapper = (header.flag6 >> 4) | (header.flag7 & 0xF0);

    nesFile->mirroring = header.flag6 & 0x01;

    if (nesFile->has_trainer) {
        file.seekg(512, std::ios::cur);
    }

    nesFile->prg_rom.resize(header.prg_rom_size * 16384);
    file.read(reinterpret_cast<char*>(nesFile->prg_rom.data()), nesFile->prg_rom.size());

    if (header.chr_rom_size > 0) {
        nesFile->chr_rom.resize(header.chr_rom_size * 8192);
        file.read(reinterpret_cast<char*>(nesFile->chr_rom.data()), nesFile->chr_rom.size());
    }

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
                mem->write(0xC000, nesFile->chr_rom.data(), nesFile->chr_rom.size());
            }

            if (!nesFile->chr_rom.empty()) {
                mem->write(0x0000, nesFile->chr_rom.data(), nesFile->chr_rom.size());
            }
            break;
        }
        default:
            ERRORLOG(error::unsupportedMapper);
    }
}

Cartridge::Cartridge(const char *path) {
    this->path = path;
}

Cartridge::~Cartridge() {
    safeDelete(nesFile);
}
