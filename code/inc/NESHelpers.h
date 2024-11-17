//
// Created by Radek on 06.11.2024.
//

#ifndef NESHELPERS_H
#define NESHELPERS_H

#include "Types.h"

namespace cpu {
    enum class AddressingMode : i8
    {
        Undefined = -1,

        ZeroPageIndexedX,
        ZeroPageIndexedY,
        AbsoluteIndexedX,
        AbsoluteIndexedY,
        IndexedIndirectX,
        IndexedIndirectY,
        Implicit,
        Accumulator,
        Immediate,
        ZeroPage,
        Absolute,
        Relative,
        Indirect
    };

    enum StatusFlag : u8
    {
        Carry = 0x01,
        Zero = 0x02,
        InterruptDisable = 0x04,
        Decimal = 0x08,
        Break = 0x10, // also B
        Always1 = 0x20,
        Overflow = 0x40,
        Negative = 0x80,
    };

    struct Registers
    {
        u8 A; //accumulator
        u8 X; //index
        u8 Y; //index
        u16 PC; //program counter
        u8 S; //stack pointer
        u8 P; //status register

        void setStatus(const StatusFlag flag){
            P |= flag;
        }

        void setStatus(const StatusFlag flag, const bool condition){
            if(condition)
                setStatus(flag);
            else
                clearStatus(flag);
        }

        void clearStatus(const StatusFlag flag){
            P &= ~flag;
        }

        void resetStatus(){
            P = 0;
        }

        bool getStatus(const StatusFlag flag) const{
            return P & flag;
        }
    };
}

namespace opcodes {
    inline bool isBranchInstruction(const u8 opcode) {
        return opcode == 0x10 || // BPL
               opcode == 0x30 || // BMI
               opcode == 0x50 || // BVC
               opcode == 0x70 || // BVS
               opcode == 0x90 || // BCC
               opcode == 0xB0 || // BCS
               opcode == 0xD0 || // BNE
               opcode == 0xF0;   // BEQ
    }

    inline bool branchConditionMet(const u8 opcode, const cpu::Registers& regs) {
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

    inline bool hasPageCrossed(const u16 baseAddr, const u8 offset) {
        const u16 newAddr = baseAddr + offset;
        return (baseAddr & 0xFF00) != (newAddr & 0xFF00);
    }

    inline bool u16AddressingMode(const cpu::AddressingMode addressingMode) {
        return addressingMode == cpu::AddressingMode::Indirect ||
               addressingMode == cpu::AddressingMode::Relative;
    }
}

#endif //NESHELPERS_H
