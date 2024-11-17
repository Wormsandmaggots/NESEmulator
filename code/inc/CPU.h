//
// Created by Radek on 01.11.2024.
//

#ifndef CPU_H
#define CPU_H

#include "Logger.h"
#include "LogMessages.h"
#include "Memory.h"
#include "Types.h"
#include "NESHelpers.h"
#include "Opcodes.h"

using namespace cpu;


class CPU {
public:

    void init();
    void reset() const;
    void cleanup();

    void execute(Instruction var);

private:
    Registers* regs = null;
    Memory* mem = null;

    u8 fetchImmediate() const;
    u8 fetchZeroPage() const;
    u8 fetchZeroPageX() const;
    u8 fetchZeroPageY() const;
    u8 fetchAbsolute() const;
    u8 fetchAbsoluteX() const;
    u8 fetchAbsoluteY() const;
    u8 fetchIndirectIndexedX() const;
    u8 fetchIndirectIndexedY() const;

    u16 fetchIndirect() const;
    u16 fetchRelative() const;

    template<typename valueType>
    valueType getFetchedValue(AddressingMode addressing_mode);
};

template<typename valueType>
valueType CPU::getFetchedValue(AddressingMode addressing_mode) {
    if constexpr (std::is_same_v<valueType, u8>) {
        switch (addressing_mode) {
            case AddressingMode::Immediate:
                return fetchImmediate();
            case AddressingMode::ZeroPage:
                return fetchZeroPage();
            case AddressingMode::ZeroPageIndexedX:
                return fetchZeroPageX();
            case AddressingMode::ZeroPageIndexedY:
                return fetchZeroPageY();
            case AddressingMode::Absolute:
                return fetchAbsolute();
            case AddressingMode::AbsoluteIndexedX:
                return fetchAbsoluteX();
            case AddressingMode::AbsoluteIndexedY:
                return fetchAbsoluteY();
            case AddressingMode::IndexedIndirectX:
                return fetchIndirectIndexedX();
            case AddressingMode::IndexedIndirectY:
                return fetchIndirectIndexedY();
            case AddressingMode::Accumulator:
                return regs->A;
            default:
                ERRORLOG(error::unsupportedAddressingModeWithU8);
                return -1;
        }
    }

    if constexpr (std::is_same<valueType, u16>::value) {
        switch (addressing_mode) {
            case AddressingMode::Indirect:
                return fetchIndirect();
            case AddressingMode::Relative:
                return fetchRelative();
            default:
                ERRORLOG(error::unsupportedAddressingModeWithU16);
                return -1;
        }
    }

    ERRORLOG(error::unsupportedTemplateType);
    return -1;
}

#endif //CPU_H
