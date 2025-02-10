//
// Created by Radek on 06.11.2024.
//
#include "NESHelpers.h"

#include "CPU.h"
#include "PPU.h"

bool opcodes::isBranchInstruction(const uint8_t opcode) {
    return opcode == 0x10 || // BPL
       opcode == 0x30 || // BMI
       opcode == 0x50 || // BVC
       opcode == 0x70 || // BVS
       opcode == 0x90 || // BCC
       opcode == 0xB0 || // BCS
       opcode == 0xD0 || // BNE
       opcode == 0xF0;   // BEQ
}

bool opcodes::branchConditionMet(const uint8_t opcode, const cpu::Registers &regs) {
    switch (opcode) {
        case 0x10: return !regs.getStatus(cpu::StatusFlag::Negative);   // BPL
        case 0x30: return regs.getStatus(cpu::StatusFlag::Negative);    // BMI
        case 0x50: return !regs.getStatus(cpu::StatusFlag::Overflow);   // BVC
        case 0x70: return regs.getStatus(cpu::StatusFlag::Overflow);    // BVS
        case 0x90: return !regs.getStatus(cpu::StatusFlag::Carry);      // BCC
        case 0xB0: return regs.getStatus(cpu::StatusFlag::Carry);       // BCS
        case 0xD0: return !regs.getStatus(cpu::StatusFlag::Zero);       // BNE
        case 0xF0: return regs.getStatus(cpu::StatusFlag::Zero);        // BEQ
        default: return false; // Nieprawidłowa instrukcja skoku
    }
}

bool opcodes::hasPageCrossed(const u16 baseAddr, const i8 offset) {
    return ((baseAddr) & 0xff00) != ((baseAddr - offset) & 0xff00);
}

bool opcodes::u16AddressingMode(const AddressingMode addressingMode) {
    return addressingMode == AddressingMode::Indirect || addressingMode == AddressingMode::Absolute;
}

bool opcodes::i8AddressingMode(const AddressingMode addressingMode) {
    return addressingMode == AddressingMode::Relative;
}

uint16_t opcodes::mergeToU16(const uint8_t low, const uint8_t high) {
    return static_cast<u16>(low) | (static_cast<u16>(high) << 8);
}

bool ppu::Registers::showBackground() const {
    return (*PPUMask) & Bit3;
}

bool ppu::Registers::showSprites() const {
    return (*PPUMask) & Bit4;
}

uint16_t ppu::Registers::spritePatternTableAddress() const {
    return (*PPUControl) & Bit3 ? 0x1000 : 0x0000;
}

uint16_t ppu::Registers::backgroundPatternTableAddress() const {
    return (*PPUControl) & Bit4 ? 0x1000 : 0x0000;
}

uint16_t ppu::Registers::nametableAddress() const {
    return 0x2000 + uint16_t(*PPUControl & 0x3) * 0x400;
}

uint8_t ppu::Registers::ppuAddressIncValue() const {
    return (*PPUControl) & Bit2 ? 32 : 1;
}

bool ppu::Registers::vblankNmi() const {
    return (*PPUControl) & Bit7;
}

void ppu::Registers::setSpriteOverflow(bool val) {
    spriteOverflow = val;
    if(val)
        (*PPUStatus) |= Bit5;
    else
        (*PPUStatus) &= ~Bit5;
}

bool ppu::Registers::getSpriteOverflow() const {
    return spriteOverflow;
}

bool ppu::Registers::use8x16Sprites() const {
    return (*PPUControl) & Bit5;
}

bool ppu::Registers::getSprite0Hit() const {
    return sprite0Hit;
}

void ppu::Registers::setSprite0Hit(bool val) {
    sprite0Hit = val;
    if(val)
        (*PPUStatus) |= Bit6;
    else
        (*PPUStatus) &= ~Bit6;
}

void ppu::Registers::setVblankFlag(bool val) {
    vblank = val;

    if(val)
        (*PPUStatus) |= Bit7;
    else
        (*PPUStatus) &= ~Bit7;
}

bool ppu::Registers::getVblankFlag() const {
    return vblank;
}

void ppu::Registers::writeLatch(uint8_t val) {
    if(protect) return;

    latch = val;
}

void ppu::Registers::writePPUSCROLL(uint8_t val) {
    writeLatch(val);

    W = !W;
    if (W)
    {
        // first write
        T = (T & 0xffe0) | (val >> 3);
        X = val & 0x7;
    }
    else
    {
        // second write
        T = (T & 0xc1f) | (uint16_t(val & 0xf8) << 2) | (uint16_t(val & 0x7) << 12);
        yScroll = val;
    }

    (*PPUScroll) = val;
}

void ppu::Registers::writePPUCTRL(uint8_t val) {
    writeLatch(val);

    T = (T & 0xf3ff) | ((val & 0x3) << 10);

    (*PPUControl) = val;
}

void ppu::Registers::writePPUAADDR(uint8_t val) {
    writeLatch(val);

    W = !W;
    if (W)
    {
        T = (T & 0x00ff) | (uint16_t(val & 0x3f) << 8);
    }
    else
    {
        T = (T & 0xff00) | val;
        V = T;
    }
}

void ppu::Registers::writePPUMASK(uint8_t val) {
    writeLatch(val);
    (*PPUMask) = val;
}

void ppu::Registers::writeOAMADDR(uint8_t val) {
    writeLatch(val);
    (*OAMAddr) = val;
}

void ppu::Registers::writeOAMDATA(uint8_t val) {
    writeLatch(val);

    oam[(*OAMAddr)] = val;
    (*OAMAddr)++;
}

void ppu::Registers::writePPUData(uint8_t val, PPU *ppu) {
    writeLatch(val);

    ppu->writeVram(V, val);
    V += ppuAddressIncValue();
}

void ppu::Registers::writeOAMDMA(uint8_t val) {
    CPU::setDMA((uint16_t(val) << 8));
}

uint8_t ppu::Registers::readPPUSTATUS() {
    u8 status = latch & 0x1f;

    if(getSprite0Hit())
        status |= Bit6;

    if(getSpriteOverflow())
        status |= Bit5;

    if(getVblankFlag())
        status |= Bit7;

    if(!protect) {
        setVblankFlag(false);
        W = false;
        writeLatch(status);
    }

    return status;
}

uint8_t ppu::Registers::readOAMDATA() {
    if (maskOAMRead)
        return 0xff;

    uint8_t val = oam[(*OAMAddr)];
    writeLatch(val);
    return val;
}

uint8_t ppu::Registers::readPPUDATA(PPU *ppu) {
    uint8_t val = *PPUData;
    uint8_t new_val = ppu->readVram(V);

    bool is_palette = ((V & 0xff00) == 0x3f00);
    if (!protect)
    {
        if (is_palette)
            *PPUData = ppu->readVram(V - 0x1000);
        else
            *PPUData = new_val;
        V += ppuAddressIncValue();
    }

    writeLatch(val);

    if (is_palette)
        return new_val;

    return val;
}

std::string Registers::toString() const {
    std::stringstream ss;

    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2);
    ss << "A: " << static_cast<int>(A) << std::endl;
    ss << "X: " << static_cast<int>(X) << std::endl;
    ss << "Y: " << static_cast<int>(Y) << std::endl;
    ss << "PC: " << static_cast<int>(PC) << std::endl;
    ss << "S: " << static_cast<int>(S) << std::endl;
    ss << "Carry: " << getStatus(Carry) << std::endl;
    ss << "Zero: " << getStatus(Zero) << std::endl;
    ss << "Interrupt: " << getStatus(InterruptDisable) << std::endl;
    ss << "Decimal: " << getStatus(Decimal) << std::endl;
    ss << "Break: " << getStatus(Break) << std::endl;
    ss << "Always1: " << getStatus(Always1) << std::endl;
    ss << "Overflow: " << getStatus(Overflow) << std::endl;
    ss << "Negative: " << getStatus(Negative) << std::endl;

    return ss.str();
}

nes_cycle_t ms_to_nes_cycle(double ms) {
    return nes_cycle_t(int64_t(NES_CLOCK_HZ / 1000 * ms));
}