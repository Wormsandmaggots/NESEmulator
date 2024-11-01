//
// Created by Radek on 01.11.2024.
//

#include "CPU.h"
#include "Utils.h"

void CPU::init() {
    regs = new Registers;
    reset();
}

void CPU::reset() const {
    regs->A = regs->X = regs->Y = 0;

    //regs->PC = (memory[0xFFFD] << 8) | memory[0xFFFC];
    regs->S = 0xFD;

    resetStatus();
    setStatus(InterruptDisable);
    setStatus(Zero);
    setStatus(Always1);
}

void CPU::cleanup() {
    safeDelete(regs);
}

void CPU::setStatus(const StatusFlag flag) const {
    regs->P |= flag;
}

void CPU::clearStatus(const StatusFlag flag) const {
    regs->P &= ~flag;
}

void CPU::resetStatus() const {
    regs->P = 0;
}

bool CPU::getStatus(StatusFlag flag) const {
    return regs->P & flag;
}
