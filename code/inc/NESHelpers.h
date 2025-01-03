//
// Created by Radek on 06.11.2024.
//

#ifndef NESHELPERS_H
#define NESHELPERS_H

#include <iomanip>
#include <sstream>
#include <string>

#include "Memory.h"
#include "Types.h"

enum Bit : u8 {
    Bit0 = 0b00000001,
    Bit1 = 0b00000010,
    Bit2 = 0b00000100,
    Bit3 = 0b00001000,
    Bit4 = 0b00010000,
    Bit5 = 0b00100000,
    Bit6 = 0b01000000,
    Bit7 = 0b10000000,
};

struct Color {
    u8 r = 0;
    u8 g = 0;
    u8 b = 0;
};

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

        std::string toString() const {
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
    };
}

namespace opcodes {
    bool isBranchInstruction(u8 opcode);
    bool branchConditionMet(u8 opcode, const cpu::Registers& regs);
    bool hasPageCrossed(u16 baseAddr, u8 offset);
    bool u16AddressingMode(cpu::AddressingMode addressingMode);
    bool i8AddressingMode(cpu::AddressingMode addressingMode);

    inline u16 mergeToU16(const u8 low, const u8 high) {
        return static_cast<u16>(low) | (static_cast<u16>(high) << 8);
    }
}

namespace ppu {
    struct Registers {
        //u8 registers[8];       // Rejestry PPU
        u8 V = 0;                 // Rejestr adresu (current VRAM address)
        u16 T = 0;                 // Rejestr adresu tymczasowego
        u8 X = 0;                  // Przesunięcie przesuwu poziomego
        u8 W = 0;                  // Flaga przełączania adresu

        u8* PPUControl = null;
        u8* PPUMask = null;
        u8* PPUStatus = null;
        u8* OAMAddr = null;
        u8* OAMData = null;
        u8* PPUScroll = null;
        u8* PPUAddr = null;
        u8* PPUData = null;
        u8* OAMDMA = null;

        int getVerticalScroll() const {
            // T = 0yyy NNYY YYYX XXXX
            return ((T >> 12) & 0b111) * 8 + ((T >> 5) & 0b111111);
        }
    };
}

#endif //NESHELPERS_H
