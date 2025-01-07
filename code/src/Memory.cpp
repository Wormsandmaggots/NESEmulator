//
// Created by Radek on 01.11.2024.
//

#include "Memory.h"

#include <cassert>

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

    beforeWrite.clear();
    beforeRead.clear();
}

u8 Memory::read(u16 addr) const {
    if(addr >= maxSize) {
        ERRORLOG(error::memoryOutOfBounds);
        return 0;
    }

    redirect_addr(addr);

    for(const auto& func : beforeRead) {
        auto result = func(addr);
        if(result != std::nullopt)
            return result.value();
    }

    return data[addr];
}

u8& Memory::getReference(uint16_t addr) {
    if(addr >= maxSize) {
        ERRORLOG(error::memoryOutOfBounds);
        return data[0];
    }

    redirect_addr(addr);
    return data[addr];
}

void Memory::write(u16 addr, u8 data) {
    if(addr >= maxSize) {
        ERRORLOG(error::memoryOutOfBounds);
        return;
    }

    if(addr == 2007)
        addr = 2007;

    redirect_addr(addr);
    bool canWrite = true;
    for (const auto& func: beforeWrite) {
        if(!func(addr, data)) {
            canWrite = false;
        }
    }

    if (canWrite)
        this->data[addr] = data;
}

void Memory::write(u16 startAddr, u8 *dataStart, u32 amount) {
    if(startAddr + amount > maxSize) {
        ERRORLOG(error::memoryOutOfBounds);
        return;
    }

    // for (int i = 0; i < amount; i++) {
    //     write(startAddr, *(dataStart + i));
    // }
    redirect_addr(startAddr);
    std::copy_n(dataStart, amount, data.begin() + startAddr);
}

void Memory::get_bytes(uint8_t *dest, uint16_t dest_size, uint16_t src_addr, size_t src_size) {
    assert(src_addr + src_size <= maxSize);
    redirect_addr(src_addr);
    memcpy_s(dest, dest_size, &data[0] + src_addr, src_size);
}

void Memory::redirect_addr(uint16_t &addr) const
{
    if ((addr & 0xE000) == 0)
    {
        // map 0x0000~0x07ff 4 times until 0x1fff
        addr &= 0x7ff;
    }
    else if ((addr & 0xE000) == 0x2000)
    {
        // map 0x2000~0x2008 every 8 bytes until 0x3fff
        addr &= 0x2007;
    }
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
