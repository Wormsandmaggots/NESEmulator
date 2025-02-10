//
// Created by Radek on 01.11.2024.
//

#include "Opcodes.h"

#include "Logger.h"
#include "LogMessages.h"
#include "NESHelpers.h"

using namespace opcodes;

void opcodes::Illegal(InstructionContext& ic) {
    ERRORLOG(error::illegalOpcode);
}

void opcodes::Unimplemented(InstructionContext& ic) {
    ERRORLOG(error::unimplementedOpcode);
}

using enum AddressingMode;

#define ILLEGAL_OPCODE Instruction{Illegal, -1, Undefined}
#define ILLEGAL_NAME "Illegal"

void initOpcodes() {
    Instructions = {
        //0
        Instruction{BRK, 7, Implicit},
        Instruction{ORA, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORA, 3, ZeroPage},
        Instruction{ASL, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PHP, 3, Implicit},
        Instruction{ORA, 2, Immediate},
        Instruction{ASL, 2, Accumulator},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORA, 4, Absolute},
        Instruction{ASL, 6, Absolute},
        ILLEGAL_OPCODE,

        //1
        Instruction{BPL, 2, Relative},
        Instruction{ORA, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORA, 4, ZeroPageIndexedX},
        Instruction{ASL, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLC, 2, Implicit},
        Instruction{ORA, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORA, 4, AbsoluteIndexedX},
        Instruction{ASL, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //2
        Instruction{JSR, 6, Absolute},
        Instruction{AND, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BIT, 3, ZeroPage},
        Instruction{AND, 3, ZeroPage},
        Instruction{ROL, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PLP, 4, Implicit},
        Instruction{AND, 2, Immediate},
        Instruction{ROL, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{BIT, 4, Absolute},
        Instruction{AND, 4, Absolute},
        Instruction{ROL, 6, Absolute},
        ILLEGAL_OPCODE,

        //3
        Instruction{BMI, 2, Relative},
        Instruction{AND, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{AND, 4, ZeroPageIndexedX},
        Instruction{ROL, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SEC, 2, Implicit},
        Instruction{AND, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{AND, 4, AbsoluteIndexedX},
        Instruction{ROL, 7,  AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //4
        Instruction{RTI, 6, Implicit},
        Instruction{EOR, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{EOR, 3, ZeroPage},
        Instruction{LSR, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PHA, 3, Implicit},
        Instruction{EOR, 2,  Immediate},
        Instruction{LSR, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{JMP, 3, Absolute},
        Instruction{EOR, 4, Absolute},
        Instruction{LSR, 6, Absolute},
        ILLEGAL_OPCODE,

        //5
        Instruction{BVC, 2, Relative},
        Instruction{EOR, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{EOR, 4, ZeroPageIndexedX},
        Instruction{LSR, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLI, 2, Implicit},
        Instruction{EOR, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{EOR, 4, AbsoluteIndexedX},
        Instruction{LSR, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //6
        Instruction{RTS, 6, Implicit},
        Instruction{ADC, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ADC, 3, ZeroPage},
        Instruction{ROR, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PLA, 4, Implicit},
        Instruction{ADC, 2, Immediate},
        Instruction{ROR, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{JMP, 5, Indirect},
        Instruction{ADC, 4, Absolute},
        Instruction{ROR, 6, Absolute},
        ILLEGAL_OPCODE,

        //7
        Instruction{BVS, 2, Relative},
        Instruction{ADC, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ADC, 4, ZeroPageIndexedX},
        Instruction{ROR, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SEI, 2, Implicit},
        Instruction{ADC, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ADC, 4, AbsoluteIndexedX},
        Instruction{ROR, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //8
        ILLEGAL_OPCODE,
        Instruction{STA, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{STY, 3, ZeroPage},
        Instruction{STA, 3, ZeroPage},
        Instruction{STX, 3, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{DEY, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{TXA, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{STY, 4, Absolute},
        Instruction{STA, 4, Absolute},
        Instruction{STX, 4, Absolute},
        ILLEGAL_OPCODE,

        //9
        Instruction{BCC, 2, Relative},
        Instruction{STA, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{STY, 4, ZeroPageIndexedX},
        Instruction{STA, 4, ZeroPageIndexedX},
        Instruction{STX, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{TYA, 2, Implicit},
        Instruction{STA, 4, AbsoluteIndexedY},
        Instruction{TXS, 2, Implicit},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{STA, 4, AbsoluteIndexedX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,

        //A
        Instruction{LDY, 2, Immediate},
        Instruction{LDA, 6, IndexedIndirectX},
        Instruction{LDX, 2, Immediate},
        ILLEGAL_OPCODE,
        Instruction{LDY, 3, ZeroPage},
        Instruction{LDA, 3, ZeroPage},
        Instruction{LDX, 3, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{TAY, 2, Implicit},
        Instruction{LDA, 2, Immediate},
        Instruction{TAX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{LDY, 4, Absolute},
        Instruction{LDA, 4, Absolute},
        Instruction{LDX, 4, Absolute},
        ILLEGAL_OPCODE,

        //B
        Instruction{BCS, 2, Relative},
        Instruction{LDA, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{LDY, 4, ZeroPageIndexedX},
        Instruction{LDA, 4, ZeroPageIndexedX},
        Instruction{LDX, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{CLV, 2, Implicit},
        Instruction{LDA, 4, AbsoluteIndexedY},
        Instruction{TSX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{LDY, 4, AbsoluteIndexedX},
        Instruction{LDA, 4, AbsoluteIndexedX},
        Instruction{LDX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,

        //C
        Instruction{CPY, 2, Immediate},
        Instruction{CMP, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{CPY, 3, ZeroPage},
        Instruction{CMP, 3, ZeroPage},
        Instruction{DEC, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{INY, 2, Implicit},
        Instruction{CMP, 2, Immediate},
        Instruction{DEX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{CPY, 4, Absolute},
        Instruction{CMP, 4, Absolute},
        Instruction{DEC, 6, Absolute},
        ILLEGAL_OPCODE,

        //D
        Instruction{BNE, 2, Relative},
        Instruction{CMP, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{CMP, 4, ZeroPageIndexedX},
        Instruction{DEC, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLD, 2, Implicit},
        Instruction{CMP, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{CMP, 4, AbsoluteIndexedX},
        Instruction{DEC, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //E
        Instruction{CPX, 2, Immediate},
        Instruction{SBC, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{CPX, 3, ZeroPage},
        Instruction{SBC, 3, ZeroPage},
        Instruction{INC, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{INX, 2, Implicit},
        Instruction{SBC, 2, Immediate},
        Instruction{NOP, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{CPX, 4, Absolute},
        Instruction{SBC, 4, Absolute},
        Instruction{INC, 6, Absolute},
        ILLEGAL_OPCODE,

        //F
        Instruction{BEQ, 2, Relative},
        Instruction{SBC, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{SBC, 4, ZeroPageIndexedX},
        Instruction{INC, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SED, 2, Implicit},
        Instruction{SBC, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{SBC, 4, AbsoluteIndexedX},
        Instruction{INC, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,
    };

    u8 opcode_address = 0;

    for (auto& instruction : Instructions) {
        instruction.opcodeAddress = opcode_address;
        opcode_address++;
    }

    Names =
    {
        "BRK",
        "ORA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ORA",
        "ASL",
        ILLEGAL_NAME,
        "PHP",
        "ORA",
        "ASL",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ORA",
        "ASL",
        ILLEGAL_NAME,

        //1
        "BPL",
        "ORA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ORA",
        "ASL",
        ILLEGAL_NAME,
        "CLC",
        "ORA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ORA",
        "ASL",
        ILLEGAL_NAME,

        //2
        "JSR",
        "AND",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "BIT",
        "AND",
        "ROL",
        ILLEGAL_NAME,
        "PLP",
        "AND",
        "ROL",
        ILLEGAL_NAME,
        "BIT",
        "AND",
        "ROL",
        ILLEGAL_NAME,

        //3
        "BMI",
        "AND",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "AND",
        "ROL",
        ILLEGAL_NAME,
        "SEC",
        "AND",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "AND",
        "ROL",
        ILLEGAL_NAME,

        //4
        "RTI",
        "EOR",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "EOR",
        "LSR",
        ILLEGAL_NAME,
        "PHA",
        "EOR",
        "LSR",
        ILLEGAL_NAME,
        "JMP",
        "EOR",
        "LSR",
        ILLEGAL_NAME,

        //5
        "BVC",
        "EOR",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "EOR",
        "LSR",
        ILLEGAL_NAME,
        "CLI",
        "EOR",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "EOR",
        "LSR",
        ILLEGAL_NAME,

        //6
        "RTS",
        "ADC",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ADC",
        "ROR",
        ILLEGAL_NAME,
        "PLA",
        "ADC",
        "ROR",
        ILLEGAL_NAME,
        "JMP",
        "ADC",
        "ROR",
        ILLEGAL_NAME,

        //7
        "BVS",
        "ADC",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ADC",
        "ROR",
        ILLEGAL_NAME,
        "SEI",
        "ADC",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "ADC",
        "ROR",
        ILLEGAL_NAME,

        //8
        ILLEGAL_NAME,
        "STA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "STY",
        "STA",
        "STX",
        ILLEGAL_NAME,
        "DEY",
        ILLEGAL_NAME,
        "TXA",
        ILLEGAL_NAME,
        "STY",
        "STA",
        "STX",
        ILLEGAL_NAME,

        //9
        "BCC",
        "STA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "STY",
        "STA",
        "STX",
        ILLEGAL_NAME,
        "TYA",
        "STA",
        "TXS",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "STA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,

        //A
        "LDY",
        "LDA",
        "LDX",
        ILLEGAL_NAME,
        "LDY",
        "LDA",
        "LDX",
        ILLEGAL_NAME,
        "TAY",
        "LDA",
        "TAX",
        ILLEGAL_NAME,
        "LDY",
        "LDA",
        "LDX",
        ILLEGAL_NAME,

        //B
        "BCS",
        "LDA",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "LDY",
        "LDA",
        "LDX",
        ILLEGAL_NAME,
        "CLV",
        "LDA",
        "TSX",
        ILLEGAL_NAME,
        "LDY",
        "LDA",
        "LDX",
        ILLEGAL_NAME,

        //C
        "CPY",
        "CMP",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "CPY",
        "CMP",
        "DEC",
        ILLEGAL_NAME,
        "INY",
        "CMP",
        "DEX",
        ILLEGAL_NAME,
        "CPY",
        "CMP",
        "DEC",
        ILLEGAL_NAME,

        //D
        "BNE",
        "CMP",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "CMP",
        "DEC",
        ILLEGAL_NAME,
        "CLD",
        "CMP",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "CMP",
        "DEC",
        ILLEGAL_NAME,

        //E
        "CPX",
        "SBC",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "CPX",
        "SBC",
        "INC",
        ILLEGAL_NAME,
        "INX",
        "SBC",
        "NOP",
        ILLEGAL_NAME,
        "CPX",
        "SBC",
        "INC",
        ILLEGAL_NAME,

        //F
        "BEQ",
        "SBC",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "SBC",
        "INC",
        ILLEGAL_NAME,
        "SED",
        "SBC",
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        ILLEGAL_NAME,
        "SBC",
        "INC",
        ILLEGAL_NAME,
    };
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

template<typename T = u8>
void setZN(const InstructionContext& ic, const T& check) {
    ic.setStatus(Zero, check == 0);
    ic.setStatus(Negative, check & Bit7);
}

void opcodes::BRK(InstructionContext& ic) {
    ic.setStatus(Break);

    ic.write(0x0100 + ic.regs->S--, (ic.regs->PC >> 8) & 0xFF); // Hi byte of PC
    ic.write(0x0100 + ic.regs->S--, ic.regs->PC & 0xFF);        // Lo byte of PC

    ic.write(0x0100 + ic.regs->S--, ic.regs->P | Break);

    ic.setStatus(InterruptDisable);

    u8 lo = ic.read(0xFFFE);
    u8 hi = ic.read(0xFFFF);
    ic.regs->PC = mergeToU16(lo, hi);
}

void opcodes::ORA(InstructionContext& ic) {
    ic.regs->SetA(ic.regs->A | ic.getValueFromAddress());

    setZN(ic, ic.regs->A);
}

void opcodes::ASL(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    if(ic.mode == Accumulator)
        operand = ic.value;

    const u8 result = operand << 1;

    if(ic.mode == Accumulator)
        ic.regs->A = result;
    else
        ic.write(ic.value, result);

    ic.setStatus(Carry, operand & Bit7);

    setZN(ic, result);
}

void opcodes::PHP(InstructionContext& ic) {
    u8 status = ic.regs->P;

    ic.write(0x100 + ic.regs->S, status);

    --ic.regs->S;
}

void opcodes::BPL(InstructionContext& ic) {
    if (!ic.getStatus(Negative)) {
        i8 offset = static_cast<i8>(ic.getValueFromAddress());
        u16 newPC = static_cast<u16>(ic.regs->PC + offset);

        ic.regs->PC = newPC;
    }
}

void opcodes::CLC(InstructionContext& ic) {
    ic.clearStatus(Carry);
}

void opcodes::JSR(InstructionContext& ic) {
    u16 address = ic.value;

    u16 returnAddress = ic.regs->PC - 1;

    ic.write(0x0100 + ic.regs->S--, (returnAddress >> 8) & 0xFF);
    ic.write(0x0100 + ic.regs->S--, returnAddress & 0xFF);

    ic.regs->PC = address;
}

void opcodes::AND(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    ic.regs->SetA(ic.regs->A & operand);

    setZN(ic, ic.regs->A);
}

void opcodes::BIT(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    ic.setStatus(Zero, (ic.regs->A & operand) == 0);

    ic.setStatus(Overflow, operand & Bit6);

    ic.setStatus(Negative, operand & Bit7);
}

void opcodes::ROL(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    bool carryIn = ic.getStatus(StatusFlag::Carry);

    u8 result = (operand << 1) | (carryIn ? 1 : 0);

    if (ic.mode != AddressingMode::Accumulator) {
        ic.write(ic.value, result);
    } else {
        ic.regs->SetA(result);
    }

    ic.setStatus(StatusFlag::Carry, operand & Bit7);

    setZN(ic, result);
}

void opcodes::PLP(InstructionContext& ic) {
    ++ic.regs->S;

    u8 status = ic.read(0x100 + ic.regs->S);

    ic.regs->P = status | Break;  // Flaga B musi być ustawiona na 1
}

void opcodes::BMI(InstructionContext& ic) {
    if (ic.getStatus(Negative)) {
        i8 offset = static_cast<i8>(ic.getValueFromAddress());

        u16 newPC = ic.regs->PC + offset;

        ic.regs->PC = newPC;
    }
}

void opcodes::SEC(InstructionContext& ic) {
    ic.setStatus(Carry);
}

void opcodes::RTI(InstructionContext& ic) {
    ++ic.regs->S;

    u8 status = ic.read(0x100 + ic.regs->S);

    ic.regs->P = (status & 0xef) | (ic.regs->P & 0x10) | 0x20;  // Flaga B (Break) jest resetowana

    ic.regs->S++;
    u8 lowByte = ic.read(0x100 + ic.regs->S);
    ic.regs->S++;
    u8 highByte = ic.read(0x100 + ic.regs->S);

    ic.regs->PC = mergeToU16(lowByte, highByte);
}

void opcodes::EOR(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    ic.regs->SetA(ic.regs->A ^ operand);

    setZN(ic, ic.regs->A);
}

void opcodes::LSR(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    if(ic.mode == Accumulator)
        operand = ic.value;

    bool carry = operand & Bit0;
    ic.setStatus(Carry, carry);

    u8 result = operand >> 1;

    if (ic.mode == AddressingMode::Accumulator) {
        ic.regs->SetA(result);
    } else {
        ic.write(ic.value, result);
    }

    setZN(ic, result);
}

void opcodes::PHA(InstructionContext& ic) {
    ic.write(0x0100 + ic.regs->S, ic.regs->A);
    --ic.regs->S;
}

void opcodes::JMP(InstructionContext& ic) {
    u16 targetAddress = ic.value;

    ic.regs->PC = targetAddress;
}

void opcodes::BVC(InstructionContext& ic) {
    if (!ic.getStatus(StatusFlag::Overflow)) {
        i8 offset = static_cast<i8>(ic.getValueFromAddress());

        u16 newAddress = ic.regs->PC + offset;

        ic.regs->PC = newAddress;
    }
}

void opcodes::CLI(InstructionContext& ic) {
    ic.clearStatus(InterruptDisable);
}

void opcodes::RTS(InstructionContext& ic) {
    ic.regs->S++;
    const u8 lowByte = ic.read(0x100 + ic.regs->S);
    ic.regs->S++;
    const u8 highByte = ic.read(0x100 + ic.regs->S);

    ic.regs->PC = mergeToU16(lowByte, highByte) + 1;
}

void opcodes::ADC(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    u8 accumulator = ic.regs->A;
    u8 carry = ic.getStatus(StatusFlag::Carry) ? 1 : 0;

    u16 result = static_cast<u16>(accumulator) + static_cast<u16>(operand) + static_cast<u16>(carry);

    ic.setStatus(Carry, result > 0xFF);

    bool overflow = (~(accumulator ^ operand) & (accumulator ^ static_cast<u8>(result)) & Bit7);
    ic.setStatus(StatusFlag::Overflow, overflow);

    ic.regs->SetA(static_cast<u8>(result));
    setZN(ic, ic.regs->A);
}

void opcodes::PLA(InstructionContext& ic) {
    ic.regs->S++;
    ic.regs->SetA(ic.read(0x100 + ic.regs->S));

    setZN(ic, ic.regs->A);
}

void opcodes::ROR(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    u8 carry = ic.regs->P & StatusFlag::Carry;
    u8 result = (operand >> 1) | (carry << 7);

    if(ic.mode == Accumulator)
        ic.regs->A = result;
    else
        ic.write(ic.value, result);

    ic.setStatus(StatusFlag::Carry, (operand & Bit0) != 0);
    setZN(ic, result);
}

void opcodes::BVS(InstructionContext& ic) {
    if (ic.getStatus(Overflow)) {
        i8 offset = static_cast<i8>(ic.getValueFromAddress());
        u16 targetAddress = ic.regs->PC + offset;

        ic.regs->PC = targetAddress;
    }
}

void opcodes::SEI(InstructionContext& ic) {
    ic.setStatus(InterruptDisable);
}

void opcodes::STA(InstructionContext& ic) {
    u16 address = 0;

    address = ic.value;

    ic.write(address, ic.regs->A);
    ic.forcePageCross = true;
}

void opcodes::STY(InstructionContext& ic) {
    u16 addr = ic.value;

    ic.write(addr, ic.regs->Y);
}

void opcodes::STX(InstructionContext& ic) {
    u16 address = ic.value;

    ic.write(address, ic.regs->X);
}

void opcodes::DEY(InstructionContext& ic) {
    --ic.regs->Y;
    setZN(ic, ic.regs->Y);
}

void opcodes::TXA(InstructionContext& ic) {
    ic.regs->SetA(ic.regs->X);
    setZN(ic, ic.regs->A);
}

void opcodes::BCC(InstructionContext& ic) {
    if (!ic.getStatus(Carry)) {
        ic.regs->PC += static_cast<i8>(ic.getValueFromAddress());
    }
}

void opcodes::TYA(InstructionContext& ic) {
    ic.regs->SetA(ic.regs->Y);
    setZN(ic, ic.regs->A);
}

void opcodes::TXS(InstructionContext& ic) {
    ic.regs->S = ic.regs->X;
}

void opcodes::LDY(InstructionContext& ic) {
    u8 value = ic.getValueFromAddress();

    ic.regs->Y = value;

    setZN(ic, ic.regs->Y);
}

void opcodes::LDA(InstructionContext& ic) {
    u8 value = ic.getValueFromAddress();

    ic.regs->SetA(value);

    setZN(ic, ic.regs->A);
}

void opcodes::LDX(InstructionContext& ic) {
    u8 value = ic.getValueFromAddress();

    ic.regs->X = value;

    setZN(ic, ic.regs->X);
}

void opcodes::TAY(InstructionContext& ic) {
    ic.regs->Y = ic.regs->A;
    setZN(ic, ic.regs->Y);
}

void opcodes::TAX(InstructionContext& ic) {
    ic.regs->X = ic.regs->A;
    setZN(ic, ic.regs->X);
}

void opcodes::BCS(InstructionContext& ic) {
    if (ic.getStatus(StatusFlag::Carry)) {
        const i8 offset = static_cast<i8>(ic.getValueFromAddress());

        ic.regs->PC += offset;
    }
}

void opcodes::CLV(InstructionContext& ic) {
    ic.clearStatus(Overflow);
}

void opcodes::TSX(InstructionContext& ic) {
    ic.regs->X = ic.regs->S;
    setZN(ic, ic.regs->X);
}

void opcodes::CPY(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    u8 Y = ic.regs->Y;
    u16 result = static_cast<u16>(Y) - static_cast<u16>(operand);

    ic.setStatus(StatusFlag::Carry, Y >= operand);

    ic.setStatus(StatusFlag::Zero, (result & 0xFF) == 0);

    ic.setStatus(StatusFlag::Negative, result & Bit7);
}

void opcodes::CMP(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    u8 A = ic.regs->A;
    u8 result = A - operand;

    ic.setStatus(StatusFlag::Carry, A >= operand);

    setZN(ic, result);
}

void opcodes::DEC(InstructionContext& ic) {
    u16 address = ic.value;
    u8 value = ic.read(address);

    u8 result = value - 1;

    ic.write(address, result);

    setZN(ic, result);
}

void opcodes::CLB(InstructionContext& ic) {
    u16 address = ic.value;
    u8 bitToClear = ic.value;

    u8 value = ic.read(address);

    u8 mask = ~(1 << bitToClear);
    value &= mask;

    ic.write(address, value);

    setZN(ic, value);
}

void opcodes::NOP(InstructionContext& ic) {
    //does nothing
    //only ads cycles
}

void opcodes::DEX(InstructionContext& ic) {
    ic.regs->X--;
    setZN(ic, ic.regs->X);
}

void opcodes::CPX(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    u8 result = ic.regs->X - operand;

    if (result & Bit7) {
        ic.setStatus(StatusFlag::Negative);
    } else {
        ic.clearStatus(StatusFlag::Negative);
    }

    if (result == 0) {
        ic.setStatus(StatusFlag::Zero);
    } else {
        ic.clearStatus(StatusFlag::Zero);
    }

    if (ic.regs->X >= operand) {
        ic.setStatus(StatusFlag::Carry);
    } else {
        ic.clearStatus(StatusFlag::Carry);
    }
}

void opcodes::SBC(InstructionContext& ic) {
    u8 operand = ic.getValueFromAddress();

    u8 carryAdjustment = ic.getStatus(StatusFlag::Carry) ? 0 : 1;
    u8 result = ic.regs->A - operand - carryAdjustment;

    if (ic.regs->A >= operand + carryAdjustment) {
        ic.setStatus(StatusFlag::Carry);
    } else {
        ic.clearStatus(StatusFlag::Carry);
    }

    if (result == 0) {
        ic.setStatus(StatusFlag::Zero);
    } else {
        ic.clearStatus(StatusFlag::Zero);
    }

    if (result & Bit7) {
        ic.setStatus(StatusFlag::Negative);
    } else {
        ic.clearStatus(StatusFlag::Negative);
    }

    if ((ic.regs->A ^ operand) & Bit7 && (ic.regs->A ^ result) & Bit7) {
        ic.setStatus(StatusFlag::Overflow);
    } else {
        ic.clearStatus(StatusFlag::Overflow);
    }

    ic.regs->SetA(result);
}

void opcodes::BEQ(InstructionContext& ic) {
    if (ic.getStatus(StatusFlag::Zero)) {
        u16 targetAddress = ic.regs->PC + static_cast<i8>(ic.getValueFromAddress());

        ic.regs->PC = targetAddress;
    }
}

void opcodes::SED(InstructionContext& ic) {
    ic.setStatus(Decimal);
}

void opcodes::INC(InstructionContext& ic) {
    u16 addr = ic.value;

    u8 operand = ic.read(addr);

    operand++;

    ic.write(addr, operand);

    setZN(ic, operand);
}

void opcodes::INX(InstructionContext& ic) {
    ic.regs->X++;
    setZN(ic, ic.regs->X);
}

void opcodes::INY(InstructionContext& ic) {
    ic.regs->Y++;
    setZN(ic, ic.regs->Y);
}

void opcodes::BNE(InstructionContext& ic) {
    if (!ic.getStatus(StatusFlag::Zero)) {

        u16 branchAddress = ic.regs->PC + static_cast<i8>(ic.getValueFromAddress());

        ic.regs->PC = branchAddress;
    }
}

void opcodes::CLD(InstructionContext& ic) {
    ic.clearStatus(Decimal);
}
