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

void ppu::Registers::writePPUData(uint8_t val, PPU *ppu) {
    writeLatch(val);

    //tutaj zapis powinien iść do vramu
    ppu->writeVram(V, val);
    //mem->write(V, val);
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

uint8_t ppu::Registers::readPPUDATA(PPU *ppu) {
    uint8_t val = *PPUData;
    uint8_t new_val = ppu->readVram(V);

    bool is_palette = ((V & 0xff00) == 0x3f00);
    if (!protect)
    {
        // for palette - the read buf is updated with the mirrored nametable address
        if (is_palette)
            *PPUData = ppu->readVram(V - 0x1000);
        else
            *PPUData = new_val;
        V += ppuAddressIncValue();
    }

    writeLatch(val);

    // palette reads returns the correct data immediately
    if (is_palette)
        return new_val;

    return val;
}
