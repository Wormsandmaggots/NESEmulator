//
// Created by Radek on 01.11.2024.
//

#include "Memory.h"

#include "Logger.h"
#include "LogMessages.h"

Memory::Memory(u32 maxSize) {
    this->maxSize = maxSize;
}

void Memory::init() {
    data.reserve(maxSize);
}

u8 Memory::read(u16 addr) const{
    if(addr >= ramSize) {
        ERRORLOG(error::memoryOutOfBonds);
        return 0;
    }

    return data[addr];
}

void Memory::write(u16 addr, u8 data) {
    if(addr >= ramSize) {
        ERRORLOG(error::memoryOutOfBonds);
        return;
    }

    this->data[addr] = data;
}

u8 Memory::operator[](u16 addr) const{
    return read(addr);
}
