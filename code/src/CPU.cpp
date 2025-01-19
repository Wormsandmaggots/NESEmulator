//
// Created by Radek on 01.11.2024.
//

#include "CPU.h"

#include <codecvt>
#include <iostream>

#include "Opcodes.h"
#include "PPU.h"
#include "Utils.h"

bool CPU::executeNMI = false;
u16 CPU::OMDDMAAddress = 0;
bool CPU::executeDMA = false;

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
        WARNLOG("NMI executed");
        pushAddress(regs->PC);
        pushByte(regs->P | 0x20);
        cycle += nes_cpu_cycle_t(7);
        regs->PC = mem->read(0xFFFA) | (mem->read(0xFFFB) << 8); // Skok do wektora NMI
        // Zresetuj flagę przerwań
        //clearVBlankFlag();

        executeNMI = false;
    }
    else if(executeDMA) {
        WARNLOG("EXECUTE OAMDMA");
        PPU::setOAMDMA(OMDDMAAddress);
        //_system->ppu()->oam_dma(_dma_addr);

        //PPU::setOAMDMA(OMDDMAAddress);

        // The entire DMA takes 513 or 514 cycles
        // http://wiki.nesdev.com/w/index.php/PPU_registers#OAMDMA
        if (cycle % 2 == nes_cpu_cycle_t(0))
            cycle += nes_cpu_cycle_t(514);
        else
            cycle += nes_cpu_cycle_t(513);

        executeDMA = false;
    }
    else {
        if(instruction.cycles < 0) {
            std::cout << instruction.cycles << std::endl;
        }

        InstructionContext ic;

        if(instruction.opcodeAddress != 173 &&
            instruction.opcodeAddress != 16 &&
            instruction.opcodeAddress != 76 &&
            instruction.opcodeAddress != 41 &&
            instruction.opcodeAddress != 240) {
            currentInstruction++;
            INFOLOG(
                std::to_string(currentInstruction) + ". " + opcodes::Names[instruction.opcodeAddress] + " " + std::
                to_string(instruction.opcodeAddress));
        }
        else {
            currentInstruction = currentInstruction;
        }

        if(instruction.opcodeAddress != 173 && instruction.opcodeAddress != 16 && regs->A == 32) {
            currentInstruction = currentInstruction;
        }

        ic.mem = mem;
        ic.regs = regs;
        ic.mode = instruction.mode;

        regs->PC++;

        //138887 -> źle ładuje A -> LDA181
        //137760. ROR 106 zapisuje pod 48 złą wartość
        if(currentInstruction == 137760)
            currentInstruction = 0;

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
    //const u16 address = (*mem)[pointer] | ((*mem)[(pointer & 0xFF00) | ((pointer + 1) & 0xFF)] << 8);
    if((pointer & 0xff) == 0xff) {
        return (*mem)[pointer] + (static_cast<u16>((*mem)[pointer & 0xff00]) << 8);
    }

    return (*mem)[pointer] | (static_cast<u16>((*mem)[pointer + 1]) << 8);
    const u16 address = opcodes::mergeToU16(pointer, (pointer + 1) % 0x100);
    return address;
}

u16 CPU::fetchIndirectIndexedX() const {
    const u8 pointer = (*mem)[regs->PC++];
    // const u16 address = (*mem)[pointer] | ((*mem)[(pointer + 1) & 0xFF] << 8);
    // return address;
    u16 val = (*mem)[(pointer + regs->X) & 0xff] + (static_cast<u16>((*mem)[(pointer + regs->X + 1) & 0xff]) << 8);
    return val;
}

u16 CPU::fetchIndirectIndexedY() {
    // const u8 pointer = (*mem)[regs->PC++];
    // const u16 address = ((*mem)[pointer] | ((*mem)[(pointer + 1) & 0xFF] << 8)) + regs->Y;
    // return (*mem)[address];
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
    //const i8 offset = static_cast<i8>(mem->read(regs->PC++));
    return regs->PC++;
}

void CPU::handleInterrupt() {
    if (executeNMI) {
        // Zapisz stan procesora
        pushAddress(regs->PC);
        pushStatus(false);
        regs->PC = mem->read(0xFFFA) | (mem->read(0xFFFB) << 8); // Skok do wektora NMI
        // Zresetuj flagę przerwań
        regs->setStatus(InterruptDisable);
        clearVBlankFlag();

        executeNMI = false;
        return;
    }

    if(executeDMA) {
        PPU::setOAMDMA(OMDDMAAddress);
        //_system->ppu()->oam_dma(_dma_addr);

        // The entire DMA takes 513 or 514 cycles
        // http://wiki.nesdev.com/w/index.php/PPU_registers#OAMDMA
        if (cycle % 2 == nes_cpu_cycle_t(0))
            cycle += nes_cpu_cycle_t(514);
        else
            cycle += nes_cpu_cycle_t(513);
    }
}

void CPU::pushStatus(bool setBreak) {
    // Zapisz flagi statusu na stosie
    regs->setStatus(Break, setBreak);
    pushByte(regs->P); // Ustawienie flagi B
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
    // Zapisz adres (adres PC) na stosie
    pushByte(address >> 8);  // Zapisz wyższy bajt
    pushByte(address & 0xFF); // Zapisz niższy bajt
}

void CPU::pushByte(uint8_t byte) {
    // Zapisz bajt na stosie (dekrementacja SP)
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

void CPU::setNMI() {
    executeNMI = true;
}

void CPU::setDMA(uint16_t omddmaAddress) {
    executeDMA = true;
    OMDDMAAddress = omddmaAddress;
}
