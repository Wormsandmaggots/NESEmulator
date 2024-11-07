//
// Created by Radek on 01.11.2024.
//

#ifndef OPCODES_H
#define OPCODES_H
#include <any>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

#include "Memory.h"
#include "NESHelpers.h"

using namespace cpu;

//https://www.masswerk.at/6502/6502_instruction_set.html#BRK

template<typename T = u8>
struct InstructionContext {
    Registers* regs = null;
    Memory* mem = null;
    std::optional<T> value = std::nullopt;

    void write(const u16 addr,const u8 data) const {
        mem->write(addr, data);
    }

    u8 read(const u16 addr) const {
        return mem->read(addr);
    }

    void setStatus(const StatusFlag flag) const {
       regs->P |= flag;
    }

    void setStatus(const StatusFlag flag, const bool condition) const {
        if(condition)
            setStatus(flag);
        else
            clearStatus(flag);
    }

    void clearStatus(const StatusFlag flag) const {
        regs->P &= ~flag;
    }

    void resetStatus() const {
        regs->P = 0;
    }

    bool getStatus(const StatusFlag flag) const {
        return regs->P & flag;
    }
};

template<typename T>
struct Instruction {
    std::function<void(InstructionContext<T>)> opcode;
    i8 cycles = -1; //cycles < 0 defines illegal opcode, unimplemented for now
    AddressingMode mode = AddressingMode::Undefined;

    bool isIllegal() const {return cycles < 0;}
    void operator()(InstructionContext<T> ic) { opcode(ic); }
};

namespace opcodes {
    //naming convention - [instruction_name][short of addressing mode]
    void BRKimpl(InstructionContext<void*> ic);
    void ORAindX(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ORAzp(InstructionContext<> ic);
    void ASLzp(InstructionContext<> ic);
    //illegal
    void PHPimpl(InstructionContext<> ic);
    void ORAimm(InstructionContext<> ic);
    void ASLa(InstructionContext<> ic);
    //illegal
    //illegal
    void ORAabs(InstructionContext<> ic);
    void ASLabs(InstructionContext<> ic);
    //illegal

    void BPLrel(InstructionContext<> ic);
    void ORAindY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ORAzpgX(InstructionContext<> ic);
    void ASLzpgX(InstructionContext<> ic);
    //illegal
    void CLCimpl(InstructionContext<> ic);
    void ORAabsY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ORAabsX(InstructionContext<> ic);
    void ASLabsX(InstructionContext<> ic);
    //illegal

    void JSRabs(InstructionContext<> ic);
    void ANDindX(InstructionContext<> ic);
    //illegal
    //illegal
    void BITzpg(InstructionContext<> ic);
    void ANDzpg(InstructionContext<> ic);
    void ROLzpg(InstructionContext<> ic);
    //illegal
    void PLPimpl(InstructionContext<> ic);
    void ANDimm(InstructionContext<> ic);
    void ROLa(InstructionContext<> ic);
    //illegal
    void BITabs(InstructionContext<> ic);
    void ANDabs(InstructionContext<> ic);
    void ROLabs(InstructionContext<> ic);
    //illegal

    void BMIrel(InstructionContext<> ic);
    void ANDindY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ANDzpgX(InstructionContext<> ic);
    void ROLzpgX(InstructionContext<> ic);
    void SECimpl(InstructionContext<> ic);
    void ANDabsY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ANDabsX(InstructionContext<> ic);
    void ROLabsX(InstructionContext<> ic);
    //illegal

    void RTIimpl(InstructionContext<> ic);
    void EORindX(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void EORzpg(InstructionContext<> ic);
    void LSRzpg(InstructionContext<> ic);
    //illegal
    void PHAimpl(InstructionContext<> ic);
    void EORimm(InstructionContext<> ic);
    void LSRa(InstructionContext<> ic);
    //illegal
    void JMPabs(InstructionContext<> ic);
    void EORabs(InstructionContext<> ic);
    void LSRabs(InstructionContext<> ic);
    //illegal

    void BVCrel(InstructionContext<> ic);
    void EORindY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void EORzpgX(InstructionContext<> ic);
    void LSRzpgX(InstructionContext<> ic);
    //illegal
    void CLIimpl(InstructionContext<> ic);
    void EORabsY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void EORabsX(InstructionContext<> ic);
    void LSRabsX(InstructionContext<> ic);
    //illegal

    void RTSimpl(InstructionContext<> ic);
    void ADCindX(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ADCzpg(InstructionContext<> ic);
    void RORzpg(InstructionContext<> ic);
    //illegal
    void PLAimpl(InstructionContext<> ic);
    void ADCimm(InstructionContext<> ic);
    void RORa(InstructionContext<> ic);
    //illegal
    void JMPind(InstructionContext<> ic);
    void ADCabs(InstructionContext<> ic);
    void RORabs(InstructionContext<> ic);
    //illegal

    void BVSrel(InstructionContext<> ic);
    void ADCindY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ADCzpgX(InstructionContext<> ic);
    void RORzpgX(InstructionContext<> ic);
    //illegal
    void SEIimpl(InstructionContext<> ic);
    void ADCabsY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void ADCabsX(InstructionContext<> ic);
    void RORabsX(InstructionContext<> ic);
    //illegal

    //illegal
    void STAindX(InstructionContext<> ic);
    //illegal
    //illegal
    void STYzpg(InstructionContext<> ic);
    void STAzpg(InstructionContext<> ic);
    void STXzpg(InstructionContext<> ic);
    //illegal
    void DEYimpl(InstructionContext<> ic);
    //illegal
    void TXAimpl(InstructionContext<> ic);
    //illegal
    void STYabs(InstructionContext<> ic);
    void STAabs(InstructionContext<> ic);
    void STXabs(InstructionContext<> ic);
    //illegal

    void BCCrel(InstructionContext<> ic);
    void STAindY(InstructionContext<> ic);
    //illegal
    //illegal
    void STYzpgX(InstructionContext<> ic);
    void STAzpgX(InstructionContext<> ic);
    void STXzpgX(InstructionContext<> ic);
    //illegal
    void TYAimpl(InstructionContext<> ic);
    void STAabsY(InstructionContext<> ic);
    void TXSimpl(InstructionContext<> ic);
    //illegal
    //illegal
    void STAabsX(InstructionContext<> ic);
    //illegal
    //illegal

    void LDYimm(InstructionContext<> ic);
    void LDAindX(InstructionContext<> ic);
    void LDXimm(InstructionContext<> ic);
    //illegal
    void LDYzpg(InstructionContext<> ic);
    void LDAzpg(InstructionContext<> ic);
    void LDXzpg(InstructionContext<> ic);
    //illegal
    void TAYimpl(InstructionContext<> ic);
    void LDAimm(InstructionContext<> ic);
    void TAXimpl(InstructionContext<> ic);
    //illegal
    void LDYabs(InstructionContext<> ic);
    void LDAabs(InstructionContext<> ic);
    void LDXabs(InstructionContext<> ic);
    //illegal

    void BCSrel(InstructionContext<> ic);
    void LDAindY(InstructionContext<> ic);
    //illegal
    //illegal
    void LDYzpgX(InstructionContext<> ic);
    void LDAzpgX(InstructionContext<> ic);
    void LDXzpgY(InstructionContext<> ic);
    //illegal
    void CLVimpl(InstructionContext<> ic);
    void LDAabsY(InstructionContext<> ic);
    void TSXimpl(InstructionContext<> ic);
    //illegal
    void LDYabsX(InstructionContext<> ic);
    void LDAabsX(InstructionContext<> ic);
    void LDXabsY(InstructionContext<> ic);
    //illegal

    void CPYimm(InstructionContext<> ic);
    void CMPindX(InstructionContext<> ic);
    //illegal
    //illegal
    void CPYzpg(InstructionContext<> ic);
    void CMPzpg(InstructionContext<> ic);
    void DECzpg(InstructionContext<> ic);
    //illegal
    void INYimpl(InstructionContext<> ic);
    void CMPimm(InstructionContext<> ic);
    void DEXimpl(InstructionContext<> ic);
    //illegal
    void CPYabs(InstructionContext<> ic);
    void CMPabs(InstructionContext<> ic);
    void DECabs(InstructionContext<> ic);
    //illegal

    void BNErel(InstructionContext<> ic);
    void CMPindY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void CMPzpgX(InstructionContext<> ic);
    void DECzpgX(InstructionContext<> ic);
    //illegal
    void CLDimpl(InstructionContext<> ic);
    void CMPabsY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void CMPabsX(InstructionContext<> ic);
    void DECabsX(InstructionContext<> ic);
    //illegal

    void CPXimm(InstructionContext<> ic);
    void SBCindX(InstructionContext<> ic);
    //illegal
    //illegal
    void CPXzpg(InstructionContext<> ic);
    void SBCzpg(InstructionContext<> ic);
    void INCzpg(InstructionContext<> ic);
    //illegal
    void INXimpl(InstructionContext<> ic);
    void SBCimm(InstructionContext<> ic);
    void NOPimpl(InstructionContext<> ic);
    //illegal
    void CPXabs(InstructionContext<> ic);
    void SBCabs(InstructionContext<> ic);
    void INCabs(InstructionContext<> ic);
    //illegal

    void BEQrel(InstructionContext<> ic);
    void SBCindY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void SBCzpgX(InstructionContext<> ic);
    void INCzpgX(InstructionContext<> ic);
    //illegal
    void SEDimpl(InstructionContext<> ic);
    void SBCabsY(InstructionContext<> ic);
    //illegal
    //illegal
    //illegal
    void SBCabsX(InstructionContext<> ic);
    void INCabsX(InstructionContext<> ic);
    //illegal

    void Illegal(InstructionContext<> ic);

    using InstructionTypes = std::variant<Instruction<u8>, Instruction<u16>, Instruction<void*>>;

    inline std::vector<InstructionTypes> Instructions
    {
        Instruction<void*>{BRKimpl, 7, AddressingMode::Implicit},
        Instruction<u8>{ORAindX, 6, AddressingMode::IndexedIndirectX}
    };
}

#endif //OPCODES_H
