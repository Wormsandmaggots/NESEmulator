//
// Created by Radek on 01.11.2024.
//

#include "Opcodes.h"

#include "Logger.h"
#include "LogMessages.h"
#include "NESHelpers.h"

using namespace opcodes;

void opcodes::BRKimpl(InstructionContext ic) {
    ic.setStatus(Break);

    // Zapis rejestru PC (program counter) na stosie (hi i lo)
    ic.write(0x0100 + ic.regs->S--, (ic.regs->PC >> 8) & 0xFF); // Hi byte of PC
    ic.write(0x0100 + ic.regs->S--, ic.regs->PC & 0xFF);        // Lo byte of PC

    // Zapis rejestru P (status) na stosie z ustawioną flagą Break
    ic.write(0x0100 + ic.regs->S--, ic.regs->P | Break);

    ic.setStatus(InterruptDisable);

    // Załadowanie adresu przerwania z wektora przerwań (0xFFFE/0xFFFF) do PC
    u8 lo = ic.read(0xFFFE);
    u8 hi = ic.read(0xFFFF);
    ic.regs->PC = (hi << 8) | lo;
}

void opcodes::ORAindX(InstructionContext ic) {
    u8 value = ic.read(ic.regs->X + ic.get<u8>()); // Dodajemy wartość X do adresu

    // Wykonaj operację OR z akumulatorem (rejestr A)
    ic.regs->A |= value;

    // Ustawienie flagi Zero (jeśli wynik jest 0)
    ic.setStatus(StatusFlag::Zero, ic.regs->A == 0);

    // Usawienie flagi Negative (jeśli bit 7 w akumulatorze jest ustawiony)
    ic.setStatus(StatusFlag::Negative, ic.regs->A & 0x80);
}

void opcodes::Illegal(InstructionContext ic) {
    ERRORLOG(error::unimplementedOpcode);
}

using enum AddressingMode;

#define ILLEGAL_OPCODE Instruction{Illegal, -1, Undefined}

void initOpcodes() {
    Instructions = {
        //0
        Instruction{BRK, 7, Implicit},
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PHP, 3, Implicit},
        Instruction{ORAindX, 2, Immediate},
        Instruction{BRKimpl, 2, Accumulator},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 6, Absolute},
        ILLEGAL_OPCODE,

        //1
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLC, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //2
        Instruction{BRKimpl, 6, Absolute},
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 3, ZeroPage},
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PLP, 4, Implicit},
        Instruction{ORAindX, 2, Immediate},
        Instruction{BRKimpl, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, Absolute},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 6, Absolute},
        ILLEGAL_OPCODE,

        //3
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SEC, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 7,  AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //4
        Instruction{RTI, 6, Implicit},
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PHA, 3, Implicit},
        Instruction{ORAindX, 2,  Immediate},
        Instruction{BRKimpl, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 3, Absolute},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 6, Absolute},
        ILLEGAL_OPCODE,

        //5
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLI, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //6
        Instruction{RTS, 6, Implicit},
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PLA, 4, Implicit},
        Instruction{ORAindX, 2, Immediate},
        Instruction{BRKimpl, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 5, Indirect},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 6, Absolute},
        ILLEGAL_OPCODE,

        //7
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SEI, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //8
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 3, ZeroPage},
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 3, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{DEY, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{TXA, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, Absolute},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 4, Absolute},
        ILLEGAL_OPCODE,

        //9
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 6, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, ZeroPageIndexedX},
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{TYA, 2, Implicit},
        Instruction{ORAindX, 5, AbsoluteIndexedY},
        Instruction{TXS, 2, Implicit},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 5, AbsoluteIndexedX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,

        //A
        Instruction{BRKimpl, 2, Immediate},
        Instruction{ORAindX, 6, IndexedIndirectX},
        Instruction{BRKimpl, 2, Immediate},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 3, ZeroPage},
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 3, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{TAY, 2, Implicit},
        Instruction{ORAindX, 2, Immediate},
        Instruction{TAX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, Absolute},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 4, Absolute},
        ILLEGAL_OPCODE,

        //B
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, ZeroPageIndexedX},
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{CLV, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        Instruction{TSX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, AbsoluteIndexedX},
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,

        //C
        Instruction{BRKimpl, 2, Immediate},
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 3, ZeroPage},
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{INY, 2, Implicit},
        Instruction{ORAindX, 2, Immediate},
        Instruction{DEX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, Absolute},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 6, Absolute},
        ILLEGAL_OPCODE,

        //D
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLD, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //E
        Instruction{BRKimpl, 2, Immediate},
        Instruction{ORAindX, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 3, ZeroPage},
        Instruction{ORAindX, 3, ZeroPage},
        Instruction{BRKimpl, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{INX, 2, Implicit},
        Instruction{ORAindX, 2, Immediate},
        Instruction{NOP, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{BRKimpl, 4, Absolute},
        Instruction{ORAindX, 4, Absolute},
        Instruction{BRKimpl, 6, Absolute},
        ILLEGAL_OPCODE,

        //F
        Instruction{BRKimpl, 2, Relative},
        Instruction{ORAindX, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, ZeroPageIndexedX},
        Instruction{BRKimpl, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SED, 2, Implicit},
        Instruction{ORAindX, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{ORAindX, 4, AbsoluteIndexedX},
        Instruction{BRKimpl, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,
    };

    u8 opcode_address = 0;

    for (auto& instruction : Instructions) {
        instruction.opcodeAddress = opcode_address;
        opcode_address++;
    }
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

template<typename T = u8>
void setZN(const InstructionContext& ic, const T& check) {
    ic.setStatus(Zero, check == 0);
    ic.setStatus(Negative, check & 0x80);
}

void opcodes::BRK(InstructionContext ic) {
    ic.setStatus(Break);

    // Zapis rejestru PC (program counter) na stosie (hi i lo)
    ic.write(0x0100 + ic.regs->S--, (ic.regs->PC >> 8) & 0xFF); // Hi byte of PC
    ic.write(0x0100 + ic.regs->S--, ic.regs->PC & 0xFF);        // Lo byte of PC

    // Zapis rejestru P (status) na stosie z ustawioną flagą Break
    ic.write(0x0100 + ic.regs->S--, ic.regs->P | Break);

    ic.setStatus(InterruptDisable);

    // Załadowanie adresu przerwania z wektora przerwań (0xFFFE/0xFFFF) do PC
    u8 lo = ic.read(0xFFFE);
    u8 hi = ic.read(0xFFFF);
    ic.regs->PC = (hi << 8) | lo;
}

void opcodes::PHP(InstructionContext ic) {
    // Wskaźnik stosu (SP) wskazuje na bieżący adres stosu
    u8 status = ic.regs->P;

    // Zmniejsz wskaźnik stosu (stack pointer)
    // Na stosie będziemy przechowywać 8-bitową wartość (rejestr P)
    --ic.regs->S;

    // Zapisz status (P) na stosie
    ic.write(ic.regs->S, status);
}

void opcodes::CLC(InstructionContext ic) {
    ic.clearStatus(Carry);
}

void opcodes::PLP(InstructionContext ic) {
    // Zwiększ wskaźnik stosu (SP) przed odczytem, ponieważ stos jest dekrementowany
    ++ic.regs->S;

    // Wczytaj wartość ze stosu (rejestr P)
    u8 status = ic.read(ic.regs->S);

    // Ustawienie flag w rejestrze P (ignorowanie flagi B)
    ic.regs->P = status | Break;  // Flaga B musi być ustawiona na 1
}

void opcodes::SEC(InstructionContext ic) {
    ic.setStatus(Carry);
}

void opcodes::RTI(InstructionContext ic) {
    ++ic.regs->S;

    // Odczytanie stanu flag procesora (P) ze stosu
    u8 status = ic.read(ic.regs->S);

    // Ustawienie flag w rejestrze P, zresetowanie flagi B (Break flag)
    ic.regs->P = status & ~Break;  // Flaga B (Break) jest resetowana

    // Odczytanie adresu powrotu (PC) ze stosu
    u8 lowByte = ic.read(ic.regs->S++);
    u8 highByte = ic.read(ic.regs->S++);

    // Składamy adres PC z dwóch bajtów
    ic.regs->PC = static_cast<u16>(lowByte) | (static_cast<u16>(highByte) << 8);
}

void opcodes::PHA(InstructionContext ic) {
    // Zmniejszenie wskaźnika stosu (S) przed zapisaniem, bo stos jest dekrementowany
    --ic.regs->S;

    // Zapisanie zawartości akumulatora (A) na stos
    ic.write(0x0100 + ic.regs->S, ic.regs->A);
}

void opcodes::CLI(InstructionContext ic) {
    ic.clearStatus(InterruptDisable);
}

void opcodes::RTS(InstructionContext ic) {
    // Odczytanie adresu powrotu ze stosu
    const u8 lowByte = ic.read(0x0100 + ic.regs->S++);
    const u8 highByte = ic.read(0x0100 + ic.regs->S++);

    // Złożenie pełnego adresu (little-endian)
    ic.regs->PC = static_cast<u16>(lowByte) | (static_cast<u16>(highByte) << 8);

    // Zwiększenie wskaźnika stosu (S) po odczycie
    ++ic.regs->S;
}

void opcodes::PLA(InstructionContext ic) {
    // Odczytanie wartości z stosu i zapisanie jej do akumulatora (A)
    ic.regs->A = ic.mem->read(0x0100 + ic.regs->S++);

    // Zwiększenie wskaźnika stosu (S) po odczycie
    ++ic.regs->S;
}

void opcodes::SEI(InstructionContext ic) {
    ic.setStatus(InterruptDisable);
}

void opcodes::DEY(InstructionContext ic) {
    // Dekrementowanie rejestru Y
    --ic.regs->Y;
    setZN(ic, ic.regs->Y);
}

void opcodes::TXA(InstructionContext ic) {
    // Przeniesienie wartości z rejestru X do rejestru A
    ic.regs->A = ic.regs->X;
    setZN(ic, ic.regs->A);
}

void opcodes::TYA(InstructionContext ic) {
    // Przeniesienie wartości z rejestru Y do rejestru A
    ic.regs->A = ic.regs->Y;
    setZN(ic, ic.regs->A);
}

void opcodes::TXS(InstructionContext ic) {
    ic.regs->S = ic.regs->X;
}

void opcodes::TAY(InstructionContext ic) {
    // Przeniesienie wartości z rejestru A do rejestru Y
    ic.regs->Y = ic.regs->A;
    setZN(ic, ic.regs->Y);
}

void opcodes::TAX(InstructionContext ic) {
    ic.regs->X = ic.regs->A;
    setZN(ic, ic.regs->X);
}

void opcodes::CLV(InstructionContext ic) {
    ic.clearStatus(Overflow);
}

void opcodes::TSX(InstructionContext ic) {
    ic.regs->X = ic.regs->S;
    setZN(ic, ic.regs->X);
}

void opcodes::NOP(InstructionContext ic) {
    //does nothing
    //only ads cycles
}

void opcodes::DEX(InstructionContext ic) {
    ic.regs->X--;
    setZN(ic, ic.regs->X);
}

void opcodes::SED(InstructionContext ic) {
    ic.setStatus(Decimal);
}

void opcodes::INX(InstructionContext ic) {
    ic.regs->X++;
    setZN(ic, ic.regs->X);
}

void opcodes::INY(InstructionContext ic) {
    ic.regs->Y++;
    setZN(ic, ic.regs->Y);
}

void opcodes::CLD(InstructionContext ic) {
    ic.clearStatus(Decimal);
}
