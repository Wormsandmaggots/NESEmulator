//
// Created by Radek on 01.11.2024.
//

#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <vector>

#include "Settings.h"

//(0x0000 do 0xFFFF):

    //0x0000 - 0x07FF: RAM (2 KB, powtarzane 4 razy, jeśli mniej).
    //0x0800 - 0x1FFF: Lustrzane odbicia RAM.
    //0x2000 - 0x3FFF: Rejestry PPU (grafika).
    //0x4000 - 0x401F: Rejestry APU (dźwięk) i I/O.
    //0x4020 - 0x5FFF: Pamięć rozszerzona (rzadko używana).
    //0x6000 - 0x7FFF: SRAM (opcjonalna, np. do zapisów stanu gry).
    //0x8000 - 0xFFFF: PRG-ROM (kod programu).

class Memory {
public:
    explicit Memory(u32 maxSize);
    explicit Memory(u16 maxSize);
    explicit Memory(u8 maxSize);
    ~Memory() = default;

    void init();

    u8 read(u16 addr) const;

    u8& getReference(u16 addr);

    void write(u16 addr, u8 data);
    void write(u16 startAddr, u8* dataStart, u32 amount);

    u8 operator[](u16 addr) const;

    std::string toString() const;

private:
    std::vector<u8> data;
    u32 maxSize;
};



#endif //MEMORY_H
