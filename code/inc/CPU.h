//
// Created by Radek on 01.11.2024.
//

#ifndef CPU_H
#define CPU_H

#include <cassert>

#include "Logger.h"
#include "LogMessages.h"
#include "Memory.h"
#include "Types.h"
#include "NESHelpers.h"

using namespace cpu;
struct Instruction;

enum Interrupt : u8 {
    None,
    NMI,
    BRK,
    IRQ
};

class CPU {
public:

    void init();
    void reset() const;
    void cleanup();

    void execute(Instruction var);
    Instruction getInstruction() const;

    Memory* getMemory() const;
    Registers* getRegisters() const;
    u16 fetch(AddressingMode addressing_mode) const;


private:
    Registers* regs = null;
    Memory* mem = null;
    Interrupt interrupt = None;

    u16 fetchImmediate() const;
    u16 fetchZeroPage() const;
    u16 fetchZeroPageX() const;
    u16 fetchZeroPageY() const;
    u16 fetchAbsolute() const;
    u16 fetchAbsoluteX() const;
    u16 fetchAbsoluteY() const;
    u16 fetchIndirectIndexedX() const;
    u16 fetchIndirectIndexedY() const;
    u16 fetchAccumulator() const;
    u16 fetchImplicit() const;

    u16 fetchIndirect() const;

    //returns offset for PC
    u16 fetchRelative() const;

    template<typename valueType>
    valueType getFetchedValue(AddressingMode addressing_mode);
    void handleInterrupt();
    void pushByte(uint8_t byte);
    void pushAddress(uint16_t address);
    void pushStatus();
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
            case AddressingMode::Absolute:
                return fetchAbsolute();
            default:
                ERRORLOG(error::unsupportedAddressingModeWithU16);
                return -1;
        }
    }

    if constexpr (std::is_same<valueType, i8>::value) {
        switch (addressing_mode) {
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
