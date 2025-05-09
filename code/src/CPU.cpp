//
// Created by Radek on 01.11.2024.
//

#include "CPU.h"
#include <iostream>

#include "Opcodes.h"
#include "PPU.h"
#include "Utils.h"
#include "LogMessages.h"

bool CPU::executeNMI = false;
u16 CPU::OMDDMAAddress = 0;
bool CPU::executeDMA = false;
bool CPU::executeIRQ = false;

void CPU::init() {

    initOpcodes();

    regs = new Registers;
    mem = new Memory(ramSize);
    mem->init();
    cycle = nes_cycle_t(0);
    reset();
}

void CPU::reset() const {
    regs->A = regs->X = regs->Y = 0;

    u16 hi = (*mem)[0xFFFD] << 8;
    u16 lo = (*mem)[0xFFFC];
    //
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
    if(executeNMI) {
        pushAddress(regs->PC);
        pushByte(regs->P);
        cycle += nes_cpu_cycle_t(7);
        regs->PC = mem->read(0xFFFA) | (mem->read(0xFFFB) << 8); // jump to NMI address

        executeNMI = false;
    }
    else if(executeDMA) {
        PPU::setOAMDMA(OMDDMAAddress);

        // http://wiki.nesdev.com/w/index.php/PPU_registers#OAMDMA
        if (cycle % 2 == nes_cpu_cycle_t(0))
            cycle += nes_cpu_cycle_t(514);
        else
            cycle += nes_cpu_cycle_t(513);

        executeDMA = false;
    }
    else if(executeIRQ) {
        if(!regs->getStatus(InterruptDisable)) {
            pushAddress(regs->PC);
            pushByte(regs->P);
            cycle += nes_cpu_cycle_t(7);
            regs->PC = mem->read(0xFFFE) | (mem->read(0xFFFF) << 8); // jump to IRQ address
            regs->clearStatus(InterruptDisable);
        }

        executeIRQ = false;
    }
    else {
        if(instruction.cycles < 0) {
            std::cout << instruction.cycles << std::endl;
            assert(false);
        }

        InstructionContext ic;

        ic.mem = mem;
        ic.regs = regs;
        ic.mode = instruction.mode;

        regs->PC++;

        ic.value = fetch(ic.mode);

        instruction(ic);

        cycle += nes_cpu_cycle_t(instruction.getCycles(ic, hasCrossedPage));
    }
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
    return (u16)(*mem)[fetchImmediate()];
}

u16 CPU::fetchZeroPageX() const {
    return (fetchZeroPage() + regs->X) & 0xff;
}

u16 CPU::fetchZeroPageY() const {
    return (fetchZeroPage() + regs->Y) & 0xff;
}

u16 CPU::fetchAbsolute() const {
    const u16 address = static_cast<u16>((*mem)[regs->PC++]) | (static_cast<u16>((*mem)[regs->PC++]) << 8);
    return address;
}

u16 CPU::fetchAbsoluteX() {
    u16 address = fetchAbsolute();
    u16 newAddress = address + regs->X;
    hasCrossedPage = ((address & 0xff00) != (newAddress & 0xff00));
    return newAddress;
}

u16 CPU::fetchAbsoluteY() {
    u16 address = fetchAbsolute();
    u16 newAddress = address + regs->Y;
    hasCrossedPage = ((address & 0xff00) != (newAddress & 0xff00));
    return newAddress;
}

u16 CPU::fetchIndirect() const {
    const u16 pointer = (*mem)[regs->PC++] | (static_cast<u16>((*mem)[regs->PC++]) << 8);
    if((pointer & 0xff) == 0xff) {
        return (*mem)[pointer] + (static_cast<u16>((*mem)[pointer & 0xff00]) << 8);
    }

    return (*mem)[pointer] | (static_cast<u16>((*mem)[pointer + 1]) << 8);
}

u16 CPU::fetchIndirectIndexedX() const {
    const u8 pointer = (*mem)[regs->PC++];
    u16 val = (*mem)[(pointer + regs->X) & 0xff] + (static_cast<u16>((*mem)[(pointer + regs->X + 1) & 0xff]) << 8);
    return val;
}

u16 CPU::fetchIndirectIndexedY() {
    const u8 pointer = (*mem)[regs->PC++];
    u16 addr = (*mem)[pointer] + (static_cast<u16>((*mem)[(pointer + 1) & 0xff]) << 8);
    u16 newAddr = addr + regs->Y;
    hasCrossedPage = ((addr & 0xff00) != (newAddr & 0xff00));
    return newAddr;
}

u16 CPU::fetchAccumulator() const {
    return regs->A;
}

u16 CPU::fetchImplicit() const {
    return -1;
}

u16 CPU::fetchRelative() const {
    return regs->PC++;
}

void CPU::pushStatus(bool setBreak) {
    regs->setStatus(Break, setBreak);
    pushByte(regs->P);
}

bool CPU::nmiEnabled() const {
    return (*mem)[0x2000] & Bit7;
}

bool CPU::vBlankFlag() const {
    return (*mem)[0x2002] & Bit7;
}

void CPU::clearVBlankFlag() {
    u8 PPUStatus = (*mem)[0x2002];
    PPUStatus &= ~Bit7;
    mem->write(0x2002, PPUStatus);
}

void CPU::pushAddress(uint16_t address) {
    pushByte(address >> 8);
    pushByte(address & 0xFF);
}

void CPU::pushByte(uint8_t byte) {
    mem->write(0x0100 + regs->S, byte);
    regs->S--;
}

u16 CPU::fetch(AddressingMode addressing_mode) {
    hasCrossedPage = false;
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

void CPU::step(nes_cycle_t new_count) {
    while(cycle < new_count) {
        execute(getInstruction());
    }
}

nes_cycle_t CPU::getCycle() const {
    return cycle;
}

void CPU::setNMI() {
    executeNMI = true;
}

void CPU::setDMA(uint16_t omddmaAddress) {
    executeDMA = true;
    OMDDMAAddress = omddmaAddress;
}

void CPU::setIRQ() {
    executeIRQ = true;
}
