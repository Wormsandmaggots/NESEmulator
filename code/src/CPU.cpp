//
// Created by Radek on 01.11.2024.
//

#include "CPU.h"
#include "Utils.h"

void CPU::init() {
    regs = new Registers;
    mem = new Memory(ramSize);
    mem->init();
    reset();
}

void CPU::reset() const {
    regs->A = regs->X = regs->Y = 0;

    regs->PC = (*mem)[0xFFFD] << 8 | (*mem)[0xFFFC];
    regs->S = 0xFD;

    regs->resetStatus();
    regs->setStatus(InterruptDisable);
    regs->setStatus(Zero);
    regs->setStatus(Always1);
}

void CPU::cleanup() {
    safeDelete(regs);
    safeDelete(mem);
}

void CPU::execute(opcodes::InstructionTypes var) {
    std::visit([this](auto&& instr) {
        if constexpr (std::is_same_v<std::decay_t<decltype(instr)>, Instruction<void*>>) {
            InstructionContext<void*> ic;
            ic.mem = mem;
            ic.regs = regs;

            instr(ic);
            INFOLOG("void*");
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(instr)>, Instruction<u8>>) {
            InstructionContext<u8> ic;
            ic.mem = mem;
            ic.regs = regs;
            ic.value = getFetchedValue<u8>(instr.mode);

            instr(ic);
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(instr)>, Instruction<u16>>) {
            InstructionContext<u16> ic;
            ic.mem = mem;
            ic.regs = regs;
            ic.value = getFetchedValue<u16>(instr.mode);

            instr(ic);
        }
    }, var);
}

u8 CPU::fetchImmediate() const {
    return (*mem)[regs->PC++];
}

u8 CPU::fetchZeroPage() const {
    const u8 address = (*mem)[regs->PC++];
    return (*mem)[address];
}

u8 CPU::fetchZeroPageX() const {
    const u8 address = ((*mem)[regs->PC++] + regs->X) & 0xFF;
    return (*mem)[address];
}

u8 CPU::fetchZeroPageY() const {
    const u8 address = ((*mem)[regs->PC++] + regs->Y) & 0xFF;
    return (*mem)[address];
}

u8 CPU::fetchAbsolute() const {
    const u16 address = (*mem)[regs->PC++] | ((*mem)[regs->PC++] << 8);
    return (*mem)[address];
}

u8 CPU::fetchAbsoluteX() const {
    const u16 address = ((*mem)[regs->PC++] | ((*mem)[regs->PC++] << 8)) + regs->X;
    return (*mem)[address];
}

u8 CPU::fetchAbsoluteY() const {
    const u16 address = ((*mem)[regs->PC++] | ((*mem)[regs->PC++] << 8)) + regs->Y;
    return (*mem)[address];
}

u16 CPU::fetchIndirect() const {
    const u16 pointer = (*mem)[regs->PC++] | ((*mem)[regs->PC++] << 8);
    const u16 address = (*mem)[pointer] | ((*mem)[(pointer & 0xFF00) | ((pointer + 1) & 0xFF)] << 8);
    return address;
}

u8 CPU::fetchIndirectIndexedX() const {
    const u8 pointer = ((*mem)[regs->PC++] + regs->X) & 0xFF;
    const u16 address = (*mem)[pointer] | ((*mem)[(pointer + 1) & 0xFF] << 8);
    return (*mem)[address];
}

u8 CPU::fetchIndirectIndexedY() const {
    const u8 pointer = (*mem)[regs->PC++];
    const u16 address = ((*mem)[pointer] | ((*mem)[(pointer + 1) & 0xFF] << 8)) + regs->Y;
    return (*mem)[address];
}

u16 CPU::fetchRelative() const {
    const i8 offset = static_cast<i8>(mem->read(regs->PC++));
    return regs->PC + offset;
}