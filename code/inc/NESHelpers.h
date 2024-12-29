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
    bool isBranchInstruction(u8 opcode);
    bool branchConditionMet(u8 opcode, const cpu::Registers& regs);
    bool hasPageCrossed(u16 baseAddr, u8 offset);
    bool u16AddressingMode(cpu::AddressingMode addressingMode);

    inline u16 mergeToU16(const u8 low, const u8 high) {
        return static_cast<u16>(low) | (static_cast<u16>(high) << 8);
    }
}

#endif //NESHELPERS_H
