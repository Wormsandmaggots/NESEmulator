//
// Created by Radek on 01.11.2024.
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "Types.h"

struct RomFileHeader
{
    char magic[4];

};

class Cartridge {
public:
    void load(const char* path);

private:
    RomFileHeader* header = null;
    const char* path = null;
};



#endif //CARTRIDGE_H
