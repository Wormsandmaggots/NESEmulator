//
// Created by Radek on 06.11.2024.
//
#include "NESHelpers.h"

#include "CPU.h"

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

void ppu::Registers::writeOAMDMA(uint8_t val) {
    CPU::setDMA((uint16_t(val) << 8));
}
