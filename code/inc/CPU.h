//
// Created by Radek on 01.11.2024.
//

#ifndef CPU_H
#define CPU_H

#include "Types.h"

enum class AddressingMode : u8
{
    ZeroPageIndexedX,
    ZeroPagedIndexY,
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
    B = 0x10,
    Always1 = 0x20,
    Overflow = 0x40,
    Negative = 0x80,
};

struct Registers
{
    u8 A;
    u8 X;
    u8 Y;
    u16 PC;
    u8 S;
    u8 P;
};


class CPU {
public:

    void init();
    void reset() const;
    void cleanup();

    void setStatus(StatusFlag) const;
    void clearStatus(StatusFlag) const;
    void resetStatus() const;
    bool getStatus(StatusFlag) const;
private:
    Registers* regs = null;
};



#endif //CPU_H
