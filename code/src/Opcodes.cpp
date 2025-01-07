//
// Created by Radek on 01.11.2024.
//

#include "Opcodes.h"

#include "Logger.h"
#include "LogMessages.h"
#include "NESHelpers.h"

using namespace opcodes;

void opcodes::Illegal(InstructionContext ic) {
    ERRORLOG(error::illegalOpcode);
}

void opcodes::Unimplemented(InstructionContext ic) {
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
        Instruction{AND, 5, IndexedIndirectX},
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
        Instruction{STA, 6, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{STY, 4, ZeroPageIndexedX},
        Instruction{STA, 4, ZeroPageIndexedX},
        Instruction{STX, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{TYA, 2, Implicit},
        Instruction{LDA, 5, AbsoluteIndexedY},
        Instruction{TXS, 2, Implicit},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{STA, 5, AbsoluteIndexedX},
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
        "LDA",
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

void opcodes::BRK(InstructionContext ic) {
    ic.setStatus(Break);

    ic.write(0x0100 + ic.regs->S--, (ic.regs->PC >> 8) & 0xFF); // Hi byte of PC
    ic.write(0x0100 + ic.regs->S--, ic.regs->PC & 0xFF);        // Lo byte of PC

    ic.write(0x0100 + ic.regs->S--, ic.regs->P | Break);

    ic.setStatus(InterruptDisable);

    u8 lo = ic.read(0xFFFE);
    u8 hi = ic.read(0xFFFF);
    ic.regs->PC = mergeToU16(lo, hi);
}

// u16 addr = 0;
//
// if(ic.holds<u16>()) {
//     addr = ic.get<u16>();
// }
// else {
//     addr = ic.get<u8>();
// }

void opcodes::ORA(InstructionContext ic) {
    //u8 operand = ic.value.value_or(0);
    //u8 operand = ic.get<u8>();
    // u16 addr = 0;
    // if(ic.holds<u16>()) {
    //     addr = ic.get<u16>();
    // }
    // else {
    //     addr = ic.get<u8>();
    // }

    // Wykonaj operację logicznego OR
    ic.regs->A |= ic.value;

    setZN(ic, ic.regs->A);
}

void opcodes::ASL(InstructionContext ic) {
    // Pobierz wartość (z akumulatora lub pamięci)
    //const u8 operand = ic.get<u8>();
    const u8 operand = ic.getValueFromAddress();
    // Ustaw flagę Carry na podstawie bitu 7
    ic.setStatus(Carry, operand & Bit7);

    // Wykonaj przesunięcie w lewo
    const u8 result = operand << 1;

    // Zapisz wynik (do akumulatora lub pamięci, w zależności od kontekstu)
    if (ic.mode == Accumulator) {
        ic.regs->A = result;
    } else {
        ic.write(operand, result);
    }

    // Ustaw flagi Z i N
    setZN(ic, result);
}

void opcodes::PHP(InstructionContext ic) {
    u8 status = ic.regs->P;

    --ic.regs->S;

    ic.write(ic.regs->S, status);
}

void opcodes::BPL(InstructionContext ic) {
    if (!ic.getStatus(Negative)) {
        // Oblicz nowy adres PC z uwzględnieniem offsetu
        //u16 val = ic.get<u16>();
        i8 offset = static_cast<i8>(ic.getValueFromAddress()); // Offset jest zapisywany jako 8-bitowa liczba ze znakiem
        u16 newPC = static_cast<u16>(ic.regs->PC + offset);

        // Ustaw nowy licznik programowy
        ic.regs->PC = newPC;
        //ic.regs->PC = ic.get<u16>();
    }
}

void opcodes::CLC(InstructionContext ic) {
    ic.clearStatus(Carry);
}

void opcodes::JSR(InstructionContext ic) {
    u16 address = ic.value;

    // Oblicz adres powrotu (PC - 1, ponieważ JSR zużywa kolejny bajt po wywołaniu)
    u16 returnAddress = ic.regs->PC - 1;

    // Zapisz adres powrotu na stosie (najpierw starszy bajt, potem młodszy bajt)
    ic.write(0x0100 + ic.regs->S--, (returnAddress >> 8) & 0xFF); // Starszy bajt
    ic.write(0x0100 + ic.regs->S--, returnAddress & 0xFF);        // Młodszy bajt

    // Ustaw licznik programu na docelowy adres
    ic.regs->PC = address;
}

void opcodes::AND(InstructionContext ic) {
    // Pobierz operand z InstructionContext
    u8 operand = ic.getValueFromAddress();

    // Wykonaj operację bitowego AND z akumulatorem
    ic.regs->A &= operand;

    // Ustaw flagi Zero (Z) i Negative (N) na podstawie wyniku
    setZN(ic, ic.regs->A);
}

void opcodes::BIT(InstructionContext ic) {
    // Pobierz operand z InstructionContext
    u8 operand = ic.getValueFromAddress();

    // Testuj, czy AND między A i operandem daje 0 (ustawienie flagi Z)
    ic.setStatus(Zero, (ic.regs->A & operand) == 0);

    // Ustaw flagę V (Overflow) na podstawie bitu 6 operandu
    ic.setStatus(Overflow, operand & Bit6);

    // Ustaw flagę N (Negative) na podstawie bitu 7 operandu
    ic.setStatus(Negative, operand & Bit7);
}

void opcodes::ROL(InstructionContext ic) {
    // Pobierz operand
    u8 operand = ic.getValueFromAddress();

    // Przechowaj poprzednią wartość Carry
    bool carryIn = ic.getStatus(StatusFlag::Carry);

    // Oblicz nowy wynik
    u16 result = (operand << 1) | (carryIn ? 1 : 0);

    // Zapisz wynik
    if (ic.mode != AddressingMode::Accumulator) {
        ic.write(ic.value, static_cast<u8>(result));
    } else {
        ic.regs->A = static_cast<u8>(result);
    }

    // Ustaw flagę Carry na podstawie najbardziej znaczącego bitu operandu
    ic.setStatus(StatusFlag::Carry, result & 0x100);

    // Ustaw flagi Zero i Negative na podstawie nowego wyniku
    ic.setStatus(StatusFlag::Zero, (result & 0xFF) == 0);
    ic.setStatus(StatusFlag::Negative, result & Bit7);
}

void opcodes::PLP(InstructionContext ic) {
    ++ic.regs->S;

    u8 status = ic.read(ic.regs->S);

    ic.regs->P = status | Break;  // Flaga B musi być ustawiona na 1
}

void opcodes::BMI(InstructionContext ic) {
    if (ic.getStatus(Negative)) {
        // Wartość przesunięcia adresu
        i8 offset = static_cast<i8>(ic.getValueFromAddress());

        // Wylicz nowy adres
        u16 newPC = ic.regs->PC + offset;

        // Zaktualizuj licznik programu
        ic.regs->PC = newPC;
    }
}

void opcodes::SEC(InstructionContext ic) {
    ic.setStatus(Carry);
}

void opcodes::RTI(InstructionContext ic) {
    ++ic.regs->S;

    u8 status = ic.read(ic.regs->S);

    ic.regs->P = status & ~Break;  // Flaga B (Break) jest resetowana

    u8 lowByte = ic.read(ic.regs->S++);
    u8 highByte = ic.read(ic.regs->S++);

    ic.regs->PC = mergeToU16(lowByte, highByte);
}

void opcodes::EOR(InstructionContext ic) {
    // Pobierz operand z kontekstu
    u8 operand = ic.getValueFromAddress();

    // Wykonaj operację XOR
    ic.regs->A ^= operand;

    // Ustawienie odpowiednich flag statusu (Zero, Negative)
    setZN(ic, ic.regs->A);
}

void opcodes::LSR(InstructionContext ic) {
    // Pobierz wartość operandu
    u8 operand = ic.getValueFromAddress();

    // Ustaw flagę Carry na podstawie najmłodszego bitu
    bool carry = operand & Bit0;
    ic.setStatus(Carry, carry);

    // Przesuń bity w prawo, najstarszy bit zostaje 0
    u8 result = operand >> 1;

    // Jeśli operacja była bezpośrednio na akumulatorze, zaktualizuj A
    if (ic.mode == AddressingMode::Accumulator) {
        ic.regs->A = result;
    } else {
        // Zapisz wynik z powrotem do pamięci, jeśli operand był w pamięci
        ic.write(ic.value, result);
    }

    // Ustaw flagi Zero (Z) i Negative (N) na podstawie wyniku
    setZN(ic, result);
}

void opcodes::PHA(InstructionContext ic) {
    --ic.regs->S;

    ic.write(0x0100 + ic.regs->S, ic.regs->A);
}

void opcodes::JMP(InstructionContext ic) {
    // Pobierz adres docelowy z InstructionContext
    u16 targetAddress = ic.value;

    // Ustaw licznik programu na adres docelowy
    ic.regs->PC = targetAddress;
}

void opcodes::BVC(InstructionContext ic) {
    if (!ic.getStatus(StatusFlag::Overflow)) {
        // Pobierz przesunięcie jako wartość ze znakiem (s8)
        i8 offset = static_cast<i8>(ic.getValueFromAddress());

        // Oblicz nowy adres na podstawie offsetu
        u16 newAddress = ic.regs->PC + offset;

        // Ustaw licznik programu na nowy adres
        ic.regs->PC = newAddress;
    }
}

void opcodes::CLI(InstructionContext ic) {
    ic.clearStatus(InterruptDisable);
}

void opcodes::RTS(InstructionContext ic) {
    const u8 lowByte = ic.read(0x0100 + ic.regs->S++);
    const u8 highByte = ic.read(0x0100 + ic.regs->S++);

    ic.regs->PC = mergeToU16(lowByte, highByte);

    ++ic.regs->S;
}

void opcodes::ADC(InstructionContext ic) {
    u8 operand = ic.getValueFromAddress();

    // Pobierz wartość akumulatora (A) i flagi Carry (C)
    u8 accumulator = ic.regs->A;
    u8 carry = ic.getStatus(StatusFlag::Carry) ? 1 : 0;

    // Wykonaj dodawanie z uwzględnieniem Carry
    u16 result = static_cast<u16>(accumulator) + static_cast<u16>(operand) + static_cast<u16>(carry);

    // Zaktualizuj flagi:
    // Flaga Carry (C) - ustawiona, jeśli wynik przekracza 8 bitów
    ic.setStatus(Carry, result > 0xFF);

    // Flaga Overflow (V) - ustawiona, jeśli wystąpił błąd liczby ze znakiem
    bool overflow = (~(accumulator ^ operand) & (accumulator ^ static_cast<u8>(result)) & Bit7);
    ic.setStatus(StatusFlag::Overflow, overflow);

    // Przypisz wynik do akumulatora (A) i ustaw flagi Z i N
    ic.regs->A = static_cast<u8>(result);
    setZN(ic, ic.regs->A);
}

void opcodes::PLA(InstructionContext ic) {
    ic.regs->A = ic.read(0x0100 + ic.regs->S++);

    ++ic.regs->S;
}

void opcodes::ROR(InstructionContext ic) {
    // Pobierz operand (jeśli dotyczy)
    u8 operand = ic.getValueFromAddress();

    // Pobierz wartość rejestru A
    u8 accumulator = ic.regs->A;

    // Jeśli tryb to bezpośredni, zaktualizuj operand
    if (ic.mode == AddressingMode::Immediate) {
        operand = accumulator; // Dla trybu Immediate operujemy na A
    }

    // Pobierz flagę Carry (C)
    bool carry = ic.getStatus(StatusFlag::Carry);

    // Wykonaj przesunięcie w prawo
    u8 result = (operand >> 1) | (carry ? Bit7 : 0);  // Bit 7 = Carry, bit 0 zyskuje Carry

    // Ustaw nową wartość w rejestrze A (jeśli dotyczy)
    if (ic.mode != AddressingMode::Immediate) {
        ic.regs->A = result;  // Zaktualizuj rejestr A, jeśli nie jest to tryb Immediate
    }

    // Ustaw flagi:
    ic.setStatus(StatusFlag::Carry, (operand & Bit0) != 0); // Zaktualizuj Carry na podstawie bitu 0
    setZN(ic, result); // Ustaw flagi Zero i Negative na podstawie wyniku
}

void opcodes::BVS(InstructionContext ic) {
    if (ic.getStatus(Overflow)) {
        // Oblicz adres skoku
        // W trybie bezpośrednim bierzemy wartość z 'value', która zawiera przesunięcie skoku
        // Adres skoku to bieżący PC + przesunięcie (relatywne)
        i8 offset = static_cast<i8>(ic.getValueFromAddress()); // Pobierz offset skoku (może być dodatni lub ujemny)
        u16 targetAddress = ic.regs->PC + offset;

        // Zaktualizuj PC na nowy adres
        ic.regs->PC = targetAddress;
    }
}

void opcodes::SEI(InstructionContext ic) {
    ic.setStatus(InterruptDisable);
}

void opcodes::STA(InstructionContext ic) {
    // Pobierz wartość adresu, gdzie należy zapisać zawartość akumulatora
    u8 address = ic.getValueFromAddress(); // Adres jest przechowywany w value, może to być u16

        // Zapisz wartość akumulatora do pamięci
    ic.write(address, ic.regs->A);
}

void opcodes::STY(InstructionContext ic) {
    // Pobierz wartość adresu, pod który należy zapisać zawartość rejestru Y
    //u16 address = ic.get<u16>(); // Adres jest przechowywany w value
    u16 addr = ic.value;

    // Zapisz wartość rejestru Y do pamięci
    ic.write(addr, ic.regs->Y);
}

void opcodes::STX(InstructionContext ic) {
    u16 address = ic.value; // Adres jest przechowywany w value

    // Zapisz wartość rejestru Y do pamięci
    ic.write(address, ic.regs->X);
}

void opcodes::DEY(InstructionContext ic) {
    --ic.regs->Y;
    setZN(ic, ic.regs->Y);
}

void opcodes::TXA(InstructionContext ic) {
    ic.regs->A = ic.regs->X;
    setZN(ic, ic.regs->A);
}

void opcodes::BCC(InstructionContext ic) {
    if (!ic.getStatus(Carry)) {
        ic.regs->PC += static_cast<i8>(ic.getValueFromAddress()); // Przesunięcie jest w trybie relatywnym
    }
}

void opcodes::TYA(InstructionContext ic) {
    ic.regs->A = ic.regs->Y;
    setZN(ic, ic.regs->A);
}

void opcodes::TXS(InstructionContext ic) {
    ic.regs->S = ic.regs->X;
}

void opcodes::LDY(InstructionContext ic) {
    // Pobierz wartość z pamięci lub natychmiastową
    u8 value = ic.getValueFromAddress();

    // Załaduj wartość do rejestru Y
    ic.regs->Y = value;

    // Ustaw flagi Z i N
    setZN(ic, ic.regs->Y);
}

void opcodes::LDA(InstructionContext ic) {
    // Pobierz wartość z pamięci lub natychmiastową
    u8 value = ic.getValueFromAddress();

    // Załaduj wartość do akumulatora A
    ic.regs->A = value;

    // Ustaw flagi Z i N
    setZN(ic, ic.regs->A);
}

void opcodes::LDX(InstructionContext ic) {
    // Pobierz wartość z pamięci lub natychmiastową
    u8 value = ic.getValueFromAddress();

    // Załaduj wartość do rejestru X
    ic.regs->X = value;

    // Ustaw flagi Z i N
    setZN(ic, ic.regs->X);
}

void opcodes::TAY(InstructionContext ic) {
    ic.regs->Y = ic.regs->A;
    setZN(ic, ic.regs->Y);
}

void opcodes::TAX(InstructionContext ic) {
    ic.regs->X = ic.regs->A;
    setZN(ic, ic.regs->X);
}

void opcodes::BCS(InstructionContext ic) {
    if (ic.getStatus(StatusFlag::Carry)) {
        // Pobierz offset (operand jest w formacie 2's complement)
        const i8 offset = static_cast<i8>(ic.getValueFromAddress());

        // Oblicz nowy PC (uwzględniając offset)
        ic.regs->PC += offset;
    }
}

void opcodes::CLV(InstructionContext ic) {
    ic.clearStatus(Overflow);
}

void opcodes::TSX(InstructionContext ic) {
    ic.regs->X = ic.regs->S;
    setZN(ic, ic.regs->X);
}

void opcodes::CPY(InstructionContext ic) {
    // Pobierz operand
    u8 operand = ic.getValueFromAddress();

    // Wykonaj operację porównania
    u8 Y = ic.regs->Y;
    u16 result = static_cast<u16>(Y) - static_cast<u16>(operand);

    // Ustaw flagę Carry, jeśli Y >= operand
    ic.setStatus(StatusFlag::Carry, Y >= operand);

    // Ustaw flagę Zero, jeśli wynik jest równy 0
    ic.setStatus(StatusFlag::Zero, (result & 0xFF) == 0);

    // Ustaw flagę Negative na podstawie najwyższego bitu wyniku
    ic.setStatus(StatusFlag::Negative, result & Bit7);
}

void opcodes::CMP(InstructionContext ic) {
    // Pobierz operand
    u8 operand = ic.getValueFromAddress();

    // Wykonaj operację porównania
    u8 A = ic.regs->A;
    u8 result = A - operand;

    // Ustaw flagę Carry, jeśli A >= operand
    ic.setStatus(StatusFlag::Carry, A >= operand);

    setZN(ic, result);
}

void opcodes::DEC(InstructionContext ic) {
    // Odczytaj wartość z pamięci pod wskazanym adresem
    u16 address = ic.value; // Adres do modyfikacji
    u8 value = ic.read(address);

    // Zmniejsz wartość o 1
    u8 result = value - 1;

    // Zapisz wynik z powrotem do pamięci
    ic.write(address, result);

    setZN(ic, result);
}

void opcodes::CLB(InstructionContext ic) {
    // Pobierz adres i bit do wyczyszczenia
    u16 address = ic.value; // Adres pamięci
    u8 bitToClear = ic.value; // Numer bitu (0-7)

    // Odczytaj wartość z pamięci
    u8 value = ic.read(address);

    // Wyczyść wskazany bit (maskowanie)
    u8 mask = ~(1 << bitToClear); // Przykładowo: bit 3 -> maska 0b11110111
    value &= mask;

    // Zapisz zmodyfikowaną wartość z powrotem do pamięci
    ic.write(address, value);

    setZN(ic, value);
}

void opcodes::NOP(InstructionContext ic) {
    //does nothing
    //only ads cycles
}

void opcodes::JNY(InstructionContext ic) {
}

void opcodes::DEX(InstructionContext ic) {
    ic.regs->X--;
    setZN(ic, ic.regs->X);
}

void opcodes::CPX(InstructionContext ic) {
    u8 operand = ic.getValueFromAddress(); // Pobierz wartość, z którą porównujemy

    // Porównanie X z operandem
    u8 result = ic.regs->X - operand;

    // Ustawienie flag
    // N (Negative) - jeśli wynik jest ujemny (zdefiniowany w Twoim rejestrze)
    if (result & Bit7) {
        ic.setStatus(StatusFlag::Negative);
    } else {
        ic.clearStatus(StatusFlag::Negative);
    }

    // Z (Zero) - jeśli X i operand są równe
    if (result == 0) {
        ic.setStatus(StatusFlag::Zero);
    } else {
        ic.clearStatus(StatusFlag::Zero);
    }

    // C (Carry) - jeśli nie ma pożyczki (tzn. jeśli X >= operand)
    if (ic.regs->X >= operand) {
        ic.setStatus(StatusFlag::Carry);
    } else {
        ic.clearStatus(StatusFlag::Carry);
    }
}

void opcodes::SBC(InstructionContext ic) {
    u8 operand = ic.getValueFromAddress();  // Pobierz operand (wartość z pamięci)

    // Uwzględnienie flagi Carry: jeżeli Carry = 1, odejmujemy 0, w przeciwnym razie 1
    u8 carryAdjustment = ic.getStatus(StatusFlag::Carry) ? 0 : 1;
    u8 result = ic.regs->A - operand - carryAdjustment;

    // Flaga C (Carry): jeśli A >= operand + carryAdjustment, to Carry = 1, w przeciwnym razie 0
    if (ic.regs->A >= operand + carryAdjustment) {
        ic.setStatus(StatusFlag::Carry);
    } else {
        ic.clearStatus(StatusFlag::Carry);
    }

    // Flaga Z (Zero): jeśli wynik jest równy 0, to flaga Zero = 1
    if (result == 0) {
        ic.setStatus(StatusFlag::Zero);
    } else {
        ic.clearStatus(StatusFlag::Zero);
    }

    // Flaga N (Negative): wynik będzie ujemny, jeśli najwyższy bit jest ustawiony (wynik < 0)
    if (result & Bit7) {
        ic.setStatus(StatusFlag::Negative);
    } else {
        ic.clearStatus(StatusFlag::Negative);
    }

    // Flaga V (Overflow): jeśli wynik przekroczył zakres (overflow), flaga jest ustawiana
    if ((ic.regs->A ^ operand) & Bit7 && (ic.regs->A ^ result) & Bit7) {
        ic.setStatus(StatusFlag::Overflow);
    } else {
        ic.clearStatus(StatusFlag::Overflow);
    }

    // Zapisz wynik w rejestrze A
    ic.regs->A = result;
}

void opcodes::JNC(InstructionContext ic) {
}

void opcodes::JNX(InstructionContext ic) {
}

void opcodes::BEQ(InstructionContext ic) {
    if (ic.getStatus(StatusFlag::Zero)) {
        // Jeśli tak, wykonaj skok do adresu zawartego w operandzie
        u16 targetAddress = ic.regs->PC + static_cast<i8>(ic.getValueFromAddress()); // Operand to offset od obecnego PC

        // Ustaw PC na docelowy adres
        ic.regs->PC = targetAddress;
    }
}

void opcodes::SED(InstructionContext ic) {
    ic.setStatus(Decimal);
}

void opcodes::INC(InstructionContext ic) {
    // Odczytujemy operand (wartość, którą chcemy inkrementować)
    u16 addr = ic.value;

    u8 operand = ic.read(addr);  // Odczytujemy wartość z pamięci pod adresem zawartym w value

    // Zwiększ wartość o 1
    operand++;

    // Zapisz wynik z powrotem do pamięci
    ic.write(addr, operand);

    // Ustaw odpowiednie flagi statusu (Zero, Negative)
    setZN(ic, operand);
}

void opcodes::INX(InstructionContext ic) {
    ic.regs->X++;
    setZN(ic, ic.regs->X);
}

void opcodes::INY(InstructionContext ic) {
    ic.regs->Y++;
    setZN(ic, ic.regs->Y);
}

void opcodes::BNE(InstructionContext ic) {
    if (!ic.getStatus(StatusFlag::Zero)) {
        // Jeżeli flaga Zero nie jest ustawiona (wynik operacji był różny od 0),
        // to wykonaj skok (Branch). Nowy adres skoku jest zawarty w ic.value.

        // Zaktualizuj program counter (PC), wykonując skok
        u16 branchAddress = ic.regs->PC + static_cast<i8>(ic.getValueFromAddress());

        // Wykonaj skok
        ic.regs->PC = branchAddress;
    }
}

void opcodes::CLD(InstructionContext ic) {
    ic.clearStatus(Decimal);
}
