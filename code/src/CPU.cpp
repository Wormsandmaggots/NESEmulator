//
// Created by Radek on 01.11.2024.
//

#include "CPU.h"

#include "Opcodes.h"
#include "Utils.h"

void CPU::init() {

    initOpcodes();

    regs = new Registers;
    mem = new Memory(ramSize);
    mem->init();
    reset();
}

void CPU::reset() const {
    regs->A = regs->X = regs->Y = 0;

    u16 hi = (*mem)[0xFFFD] << 8;
    u16 lo = (*mem)[0xFFFC];

    regs->PC =  hi | lo;
    regs->S = 0xFD;

    regs->resetStatus();
    regs->setStatus(InterruptDisable);
    regs->setStatus(Always1);
}

void CPU::cleanup() {
    safeDelete(regs);
    safeDelete(mem);
}

void CPU::execute(Instruction instruction) {
    handleInterrupt();
    InstructionContext ic;
    INFOLOG(opcodes::Names[instruction.opcodeAddress] +  " " + std::to_string(instruction.opcodeAddress));
    ic.mem = mem;
    ic.regs = regs;
    ic.mode = instruction.mode;

    regs->PC++;

    ic.value = fetch(ic.mode);

    instruction(ic);
}

Instruction CPU::getInstruction() const {
    return opcodes::Instructions[(*mem)[regs->PC]];
}

Memory * CPU::getMemory() const {
    return mem;
}

Registers * CPU::getRegisters() const {
    return regs;
}

u16 CPU::fetchImmediate() const {
    return regs->PC++;
}

u16 CPU::fetchZeroPage() const {
    return (*mem)[fetchImmediate()];
}

u16 CPU::fetchZeroPageX() const {
    return (fetchZeroPage() + regs->X) % 0x100;
}

u16 CPU::fetchZeroPageY() const {
    return (fetchZeroPage() + regs->Y) % 0x100;
}

u16 CPU::fetchAbsolute() const {
    const u16 address = (*mem)[regs->PC++] | ((*mem)[regs->PC++] << 8);
    return address;
}

u16 CPU::fetchAbsoluteX() const {
    const u16 address = (fetchAbsolute() + regs->X) % 0x100;
    return address;
}

u16 CPU::fetchAbsoluteY() const {
    const u16 address = (fetchAbsolute() + regs->Y) %  0x100;
    return address;
}

u16 CPU::fetchIndirect() const {
    const u8 pointer = (*mem)[regs->PC++] | ((*mem)[regs->PC++] << 8);
    //const u16 address = (*mem)[pointer] | ((*mem)[(pointer & 0xFF00) | ((pointer + 1) & 0xFF)] << 8);
    const u16 address = opcodes::mergeToU16(pointer, (pointer + 1) % 0x100);
    return address;
}

u16 CPU::fetchIndirectIndexedX() const {
    // const u8 pointer = ((*mem)[regs->PC++] + regs->X) & 0xFF;
    // const u16 address = (*mem)[pointer] | ((*mem)[(pointer + 1) & 0xFF] << 8);
    // return address;
    return (fetchIndirect() + regs->X) % 0x100;
}

u16 CPU::fetchIndirectIndexedY() const {
    // const u8 pointer = (*mem)[regs->PC++];
    // const u16 address = ((*mem)[pointer] | ((*mem)[(pointer + 1) & 0xFF] << 8)) + regs->Y;
    // return (*mem)[address];
    return (fetchIndirect() + regs->Y) % 0x100;
}

u16 CPU::fetchAccumulator() const {
    return regs->A;
}

u16 CPU::fetchImplicit() const {
    return -1;
}

u16 CPU::fetchRelative() const {
    //const i8 offset = static_cast<i8>(mem->read(regs->PC++));
    return regs->PC++;
}

void CPU::handleInterrupt() {
    if (interrupt == NMI) {
        interrupt = None;;
        // Zapisz stan procesora
        pushStatus();
        pushAddress(regs->PC);
        regs->PC = mem->read(0xFFFA) | (mem->read(0xFFFB) << 8); // Skok do wektora NMI
        // Zresetuj flagę przerwań
        regs->setStatus(InterruptDisable);
        return;
    }

    // Sprawdź, czy jest aktywne przerwanie IRQ
    if (interrupt == IRQ && !regs->getStatus(InterruptDisable)) { // Flaga Interrupt Disable (I) = 0
        interrupt = None;
        // Zapisz stan procesora
        pushStatus();
        pushAddress(regs->PC);
        regs->PC = mem->read(0xFFFE) | (mem->read(0xFFFF) << 8); // Skok do wektora IRQ
        // Zresetuj flagę przerwań
        regs->setStatus(InterruptDisable);
        return;
    }
}

void CPU::pushStatus() {
    // Zapisz flagi statusu na stosie
    pushByte(regs->P | (1 << 5)); // Ustawienie flagi B
}

void CPU::pushAddress(uint16_t address) {
    // Zapisz adres (adres PC) na stosie
    pushByte(address >> 8);  // Zapisz wyższy bajt
    pushByte(address & 0xFF); // Zapisz niższy bajt
}

void CPU::pushByte(uint8_t byte) {
    // Zapisz bajt na stosie (dekrementacja SP)
    mem->write(0x0100 + regs->S, byte);
    regs->S--;
}

u16 CPU::fetch(AddressingMode addressing_mode) const {
    switch(addressing_mode) {
        case AddressingMode::Implicit:
            return fetchImplicit();
        case AddressingMode::Absolute:
            return fetchAbsolute();
        case AddressingMode::Indirect:
            return fetchIndirect();
        case AddressingMode::Accumulator:
            return fetchAccumulator();
        case AddressingMode::Relative:
            return fetchRelative();
        case AddressingMode::Immediate:
            return fetchImmediate();
        case AddressingMode::ZeroPage:
            return fetchZeroPage();
        case AddressingMode::AbsoluteIndexedX:
            return fetchAbsoluteX();
        case AddressingMode::AbsoluteIndexedY:
            return fetchAbsoluteY();
        case AddressingMode::IndexedIndirectX:
            return fetchIndirectIndexedX();
        case AddressingMode::IndexedIndirectY:
            return fetchIndirectIndexedY();
        case AddressingMode::ZeroPageIndexedX:
            return fetchZeroPageX();
        case AddressingMode::ZeroPageIndexedY:
            return fetchZeroPageY();
        default: {
            ERRORLOG(error::unsupportedAddressingModeWithU16);
            return -1;
        }
    }
}
