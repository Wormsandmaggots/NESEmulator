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

    template<typename valueType = u8>
    void execute(Instruction<valueType> instruction);

    //template<typename valueType>
    void execute(opcodes::InstructionTypes var);

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
            case AddressingMode::Absolute:
                return fetchAbsolute();
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

template<typename valueType>
void CPU::execute(Instruction<valueType> instruction) {
    if constexpr (std::is_same_v<valueType, void*>) {
        InstructionContext<void*> context;
        context.mem = this->mem;
        context.regs = this->regs;

        instruction(context);
    }
    else {
        InstructionContext<valueType> context;
        context.mem = this->mem;
        context.regs = this->regs;
        context.value = getFetchedValue<valueType>(instruction.mode);

        instruction(context);
    }
}

// template<typename T>
// void CPU::execute(opcodes::InstructionTypes var) {
//     std::visit([this](auto&& instr) {
//         if constexpr (sizeof(instr) == sizeof(Instruction<void*>)) {
//             InstructionContext<T> ic;
//             ic.mem = mem;
//             ic.regs = regs;
//
//             instr(ic);
//             INFOLOG("void*");
//         }
//         else if constexpr (sizeof(instr) == sizeof(Instruction<u8>)) {
//             InstructionContext<T> ic;
//             ic.mem = mem;
//             ic.regs = regs;
//             ic.value = getFetchedValue<T>(instr.mode);
//
//             instr(ic);
//             INFOLOG("u8");
//         }
//         else if constexpr (sizeof(instr) == sizeof(Instruction<u16>)) {
//             InstructionContext<T> ic;
//             ic.mem = mem;
//             ic.regs = regs;
//             ic.value = getFetchedValue<T>(instr.mode);
//
//             instr(ic);
//             INFOLOG("u16");
//         }
//
//     }, var);
// }

#endif //CPU_H
