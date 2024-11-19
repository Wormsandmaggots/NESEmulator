//
// Created by Radek on 01.11.2024.
//

#ifndef MEMORY_H
#define MEMORY_H

#include <vector>

#include "Settings.h"

class Memory {
public:
    Memory(u32 maxSize);
    ~Memory() = default;

    void init();

    u8 read(u16 addr) const;
    void write(u16 addr, u8 data);

    u8 operator[](u16 addr) const;

private:
    std::vector<u8> data;
    u32 maxSize;
};



#endif //MEMORY_H
