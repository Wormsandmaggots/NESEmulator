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
    void step(nes_cycle_t new_count);

    static void setNMI();

private:
    static bool executeNMI;

    Registers* regs = null;
    Memory* mem = null;
    Interrupt interrupt = None;

    nes_cycle_t cycle;

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

    void handleInterrupt();
    void pushByte(uint8_t byte);
    void pushAddress(uint16_t address);
    void pushStatus(bool setBreak = true);

    bool nmiEnabled() const;
    bool vBlankFlag() const;
    void clearVBlankFlag();
};

#endif //CPU_H
