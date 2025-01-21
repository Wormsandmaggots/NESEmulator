//
// Created by Radek on 01.11.2024.
//

#ifndef OPCODES_H
#define OPCODES_H
#include <functional>
#include <variant>
#include <vector>

#include "Logger.h"
#include "LogMessages.h"
#include "Memory.h"
#include "NESHelpers.h"

using namespace cpu;

//https://www.masswerk.at/6502/6502_instruction_set.html#BRK

struct InstructionContext {
    Registers* regs = null;
    Memory* mem = null;
    AddressingMode mode = AddressingMode::Undefined;
    //std::variant<u8, i8, u16, void*> value;
    u16 value = 0;
    bool forcePageCross = false;

    void write(const u16 addr,const u8 data) const {
        mem->write(addr, data);
    }

    u8 read(const u16 addr) const {
        return mem->read(addr);
    }

    void setStatus(const StatusFlag flag) const {
       regs->setStatus(flag);
    }

    void setStatus(const StatusFlag flag, const bool condition) const {
        if(condition)
            setStatus(flag);
        else
            clearStatus(flag);
    }

    void clearStatus(const StatusFlag flag) const {
        regs->clearStatus(flag);
    }

    void resetStatus() const {
        regs->P = 0;
    }

    bool getStatus(const StatusFlag flag) const {
        return regs->getStatus(flag);
    }

    //zmiana nazwy na coś bardziej suitable
    u8 getValueFromAddress() const{ return mode == AddressingMode::Accumulator ? value : read(value); }

    void pushByte(uint8_t byte) const {
        // Zapisz bajt na stosie (dekrementacja SP)
        mem->write(0x0100 + regs->S, byte);
        regs->S--;
    }

    void pushStatus() const {
        // Zapisz flagi statusu na stosie
        pushByte(regs->P | (1 << 5)); // Ustawienie flagi B
    }

    void pushAddress(uint16_t address) const {
        // Zapisz adres (adres PC) na stosie
        pushByte(address >> 8);  // Zapisz wyższy bajt
        pushByte(address & 0xFF); // Zapisz niższy bajt
    }

    // template<typename T>
    // T get() {
    //     if(std::holds_alternative<T>(value)) {
    //         return std::get<T>(value);
    //     }
    //
    //     ERRORLOG(error::invalidVariantTypeGet);
    //     return 0;
    // }
    //
    // template<typename T>
    // bool holds() {
    //     return std::holds_alternative<T>(value);
    // }
};

struct Instruction {
    std::function<void(InstructionContext&)> opcode;
    i8 cycles = -1; //cycles < 0 defines illegal opcode, unimplemented for now
    AddressingMode mode = AddressingMode::Undefined;
    u8 opcodeAddress = 0x0;

    bool isIllegal() const { return cycles < 0 && mode != AddressingMode::Undefined; }
    void operator()(InstructionContext& ic) const {
        opcode(ic);
    }

    i16 getCycles(InstructionContext& ic, bool hasCrossedPage = false) const
    {
        using namespace opcodes;
        using enum AddressingMode;

        u8 currentCycles = cycles;

        if(cycles <= illegalCycles)
            return cycles;

        if(isBranchInstruction(opcodeAddress) && branchConditionMet(opcodeAddress, *ic.regs)) {
            currentCycles+=1;

            if((ic.regs->PC & 0xff00) != ((ic.regs->PC - (i8)ic.getValueFromAddress()) & 0xff00))
                currentCycles+=1;

            return currentCycles;
        }

        if(hasCrossedPage) {
            currentCycles+=1;
            return currentCycles;
        }

        if(ic.forcePageCross) {
            if(mode == AbsoluteIndexedX || mode == AbsoluteIndexedY || mode == IndexedIndirectY)
                currentCycles+=1;
        }

        // if(mode == AbsoluteIndexedX || mode == AbsoluteIndexedY || mode == IndexedIndirectY) {
        //     const bool pageCross = hasCrossedPage || ic.forcePageCross;
        //
        //     if(pageCross)
        //         currentCycles+=1;
        // }

        return currentCycles;
    }
};

void initOpcodes();

namespace opcodes {
    void BRK(InstructionContext& ic);
    void ORA(InstructionContext& ic);
    void ASL(InstructionContext& ic);
    void PHP(InstructionContext& ic);//4

    void BPL(InstructionContext& ic);
    void CLC(InstructionContext& ic);//6

    void JSR(InstructionContext& ic);
    void AND(InstructionContext& ic);
    void BIT(InstructionContext& ic);
    void ROL(InstructionContext& ic);
    void PLP(InstructionContext& ic);//11

    void BMI(InstructionContext& ic);
    void SEC(InstructionContext& ic);//13

    void RTI(InstructionContext& ic);
    void EOR(InstructionContext& ic);
    void LSR(InstructionContext& ic);
    void PHA(InstructionContext& ic);
    void JMP(InstructionContext& ic);//18

    void BVC(InstructionContext& ic);
    void CLI(InstructionContext& ic);//20

    void RTS(InstructionContext& ic);
    void ADC(InstructionContext& ic);
    void PLA(InstructionContext& ic);
    void ROR(InstructionContext& ic);//24

    void BVS(InstructionContext& ic);
    void SEI(InstructionContext& ic);//26

    void STA(InstructionContext& ic);
    void STY(InstructionContext& ic);
    void STX(InstructionContext& ic);
    void DEY(InstructionContext& ic);
    void TXA(InstructionContext& ic);//31

    void BCC(InstructionContext& ic);
    void TYA(InstructionContext& ic);
    void TXS(InstructionContext& ic);//34

    void LDY(InstructionContext& ic);
    void LDA(InstructionContext& ic);
    void LDX(InstructionContext& ic);
    void TAY(InstructionContext& ic);
    void TAX(InstructionContext& ic);//39

    void BCS(InstructionContext& ic);
    void CLV(InstructionContext& ic);
    void TSX(InstructionContext& ic);//42

    void CPY(InstructionContext& ic);
    void CMP(InstructionContext& ic);
    void DEC(InstructionContext& ic);
    void CLB(InstructionContext& ic);
    void NOP(InstructionContext& ic);
    void JNY(InstructionContext& ic);
    void DEX(InstructionContext& ic);//49

    void CPX(InstructionContext& ic);
    void SBC(InstructionContext& ic);
    void JNC(InstructionContext& ic);
    void JNX(InstructionContext& ic);
    void BEQ(InstructionContext& ic);
    void SED(InstructionContext& ic);//55

    void INC(InstructionContext& ic);
    void INX(InstructionContext& ic);
    void INY(InstructionContext& ic);
    void BNE(InstructionContext& ic);
    void CLD(InstructionContext& ic);//60


    void Illegal(InstructionContext& ic);
    void Unimplemented(InstructionContext& ic);

    inline std::vector<Instruction> Instructions;
    inline std::vector<std::string> Names;
}

#endif //OPCODES_H
