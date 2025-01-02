//
// Created by Radek on 01.11.2024.
//

#include "Memory.h"

#include "Logger.h"
#include "LogMessages.h"

Memory::Memory(u32 maxSize) {
    this->maxSize = maxSize;
}

Memory::Memory(u16 maxSize) {
    this->maxSize = maxSize;
}

Memory::Memory(u8 maxSize) {
    this->maxSize = maxSize;
}

void Memory::init() {
    data.reserve(maxSize);
    data.resize(maxSize, 0);
}

u8 Memory::read(u16 addr) const{
    if(addr >= maxSize) {
        ERRORLOG(error::memoryOutOfBonds);
        return 0;
    }

    return data[addr];
}

void Memory::write(u16 addr, u8 data) {
    if(addr >= maxSize) {
        ERRORLOG(error::memoryOutOfBonds);
        return;
    }

    this->data[addr] = data;
}

void Memory::write(u16 startAddr, u8 *dataStart, u32 amount) {
    if(startAddr + amount > maxSize) {
        ERRORLOG(error::memoryOutOfBonds);
        return;
    }

    std::copy_n(dataStart, amount, data.begin() + startAddr);
}

u8 Memory::operator[](u16 addr) const{
    return read(addr);
}

std::string Memory::toString() const {
    std::stringstream ss;

    for(int i = 0; i < data.size(); ++i) {
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]) << " ";

        if ((i + 1) % 16 == 0) {
            ss << "\n";
        }
    }

    return ss.str();
}
