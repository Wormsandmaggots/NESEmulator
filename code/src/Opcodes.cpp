//
// Created by Radek on 01.11.2024.
//

#include "Opcodes.h"

#include "Logger.h"
#include "LogMessages.h"
#include "NESHelpers.h"

using namespace opcodes;

enum Bit : u8 {
    Bit0 = 0b00000001,
    Bit1 = 0b00000010,
    Bit2 = 0b00000100,
    Bit3 = 0b00001000,
    Bit4 = 0b00010000,
    Bit5 = 0b00100000,
    Bit6 = 0b01000000,
    Bit7 = 0b10000000,
};

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
    ERRORLOG(error::illegalOpcode);
}

void opcodes::Unimplemented(InstructionContext ic) {
    ERRORLOG(error::unimplementedOpcode);
}

using enum AddressingMode;

#define ILLEGAL_OPCODE Instruction{Illegal, -1, Undefined}

void initOpcodes() {
    Instructions = {
        //0
        Instruction{BRK, 7, Implicit},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PHP, 3, Implicit},
        Instruction{Unimplemented, 2, Immediate},
        Instruction{Unimplemented, 2, Accumulator},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 6, Absolute},
        ILLEGAL_OPCODE,

        //1
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLC, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //2
        Instruction{Unimplemented, 6, Absolute},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PLP, 4, Implicit},
        Instruction{Unimplemented, 2, Immediate},
        Instruction{Unimplemented, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 6, Absolute},
        ILLEGAL_OPCODE,

        //3
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SEC, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 7,  AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //4
        Instruction{RTI, 6, Implicit},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PHA, 3, Implicit},
        Instruction{Unimplemented, 2,  Immediate},
        Instruction{Unimplemented, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 6, Absolute},
        ILLEGAL_OPCODE,

        //5
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLI, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //6
        Instruction{RTS, 6, Implicit},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{PLA, 4, Implicit},
        Instruction{Unimplemented, 2, Immediate},
        Instruction{Unimplemented, 2, Accumulator},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 5, Indirect},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 6, Absolute},
        ILLEGAL_OPCODE,

        //7
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SEI, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //8
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{DEY, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{TXA, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        ILLEGAL_OPCODE,

        //9
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 6, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{TYA, 2, Implicit},
        Instruction{Unimplemented, 5, AbsoluteIndexedY},
        Instruction{TXS, 2, Implicit},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 5, AbsoluteIndexedX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,

        //A
        Instruction{Unimplemented, 2, Immediate},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        Instruction{Unimplemented, 2, Immediate},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{TAY, 2, Implicit},
        Instruction{Unimplemented, 2, Immediate},
        Instruction{TAX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        ILLEGAL_OPCODE,

        //B
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 4, ZeroPageIndexedY},
        ILLEGAL_OPCODE,
        Instruction{CLV, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        Instruction{TSX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,

        //C
        Instruction{Unimplemented, 2, Immediate},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{INY, 2, Implicit},
        Instruction{Unimplemented, 2, Immediate},
        Instruction{DEX, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 6, Absolute},
        ILLEGAL_OPCODE,

        //D
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{CLD, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 7, AbsoluteIndexedX},
        ILLEGAL_OPCODE,

        //E
        Instruction{Unimplemented, 2, Immediate},
        Instruction{Unimplemented, 6, IndexedIndirectX},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 3, ZeroPage},
        Instruction{Unimplemented, 5, ZeroPage},
        ILLEGAL_OPCODE,
        Instruction{INX, 2, Implicit},
        Instruction{Unimplemented, 2, Immediate},
        Instruction{NOP, 2, Implicit},
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 4, Absolute},
        Instruction{Unimplemented, 6, Absolute},
        ILLEGAL_OPCODE,

        //F
        Instruction{Unimplemented, 2, Relative},
        Instruction{Unimplemented, 5, IndexedIndirectY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, ZeroPageIndexedX},
        Instruction{Unimplemented, 6, ZeroPageIndexedX},
        ILLEGAL_OPCODE,
        Instruction{SED, 2, Implicit},
        Instruction{Unimplemented, 4, AbsoluteIndexedY},
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        ILLEGAL_OPCODE,
        Instruction{Unimplemented, 4, AbsoluteIndexedX},
        Instruction{Unimplemented, 7, AbsoluteIndexedX},
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

void opcodes::ORA(InstructionContext ic) {
    //u8 operand = ic.value.value_or(0);
    u8 operand = ic.get<u8>();

    // Wykonaj operację logicznego OR
    ic.regs->A |= operand;

    setZN(ic, ic.regs->A);
}

void opcodes::ASL(InstructionContext ic) {
    // Pobierz wartość (z akumulatora lub pamięci)
    const u8 operand = ic.get<u8>();

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
        i8 offset = static_cast<i8>(ic.get<u8>()); // Offset jest zapisywany jako 8-bitowa liczba ze znakiem
        u16 newPC = static_cast<u16>(ic.regs->PC + offset);

        // Ustaw nowy licznik programowy
        ic.regs->PC = newPC;
    }
}

void opcodes::CLC(InstructionContext ic) {
    ic.clearStatus(Carry);
}

void opcodes::JSR(InstructionContext ic) {
    u16 address = ic.get<u16>();

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
    u8 operand = ic.get<u8>();

    // Wykonaj operację bitowego AND z akumulatorem
    ic.regs->A &= operand;

    // Ustaw flagi Zero (Z) i Negative (N) na podstawie wyniku
    setZN(ic, ic.regs->A);
}

void opcodes::BIT(InstructionContext ic) {
    // Pobierz operand z InstructionContext
    u8 operand = ic.get<u8>();

    // Testuj, czy AND między A i operandem daje 0 (ustawienie flagi Z)
    ic.setStatus(Zero, (ic.regs->A & operand) == 0);

    // Ustaw flagę V (Overflow) na podstawie bitu 6 operandu
    ic.setStatus(Overflow, operand & Bit6);

    // Ustaw flagę N (Negative) na podstawie bitu 7 operandu
    ic.setStatus(Negative, operand & Bit7);
}

void opcodes::ROL(InstructionContext ic) {
    // Pobierz operand
    u8 operand = ic.get<u8>();

    // Przechowaj poprzednią wartość Carry
    bool carryIn = ic.getStatus(StatusFlag::Carry);

    // Oblicz nowy wynik
    u16 result = (operand << 1) | (carryIn ? 1 : 0);

    // Zapisz wynik
    if (ic.mode != AddressingMode::Accumulator) {
        ic.write(ic.get<u8>(), static_cast<u8>(result));
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
        u8 offset = ic.get<u8>();

        // Wylicz nowy adres
        u16 newPC = ic.regs->PC + static_cast<i8>(offset);

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
    u8 operand = ic.get<u8>();

    // Wykonaj operację XOR
    ic.regs->A ^= operand;

    // Ustawienie odpowiednich flag statusu (Zero, Negative)
    setZN(ic, ic.regs->A);
}

void opcodes::LSR(InstructionContext ic) {
    // Pobierz wartość operandu
    u8 operand = ic.get<u8>();

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
        ic.write(ic.get<u8>(), result);
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
    u16 targetAddress = ic.get<u16>();

    // Ustaw licznik programu na adres docelowy
    ic.regs->PC = targetAddress;
}

void opcodes::BVC(InstructionContext ic) {
    if (!ic.getStatus(StatusFlag::Overflow)) {
        // Pobierz przesunięcie jako wartość ze znakiem (s8)
        i8 offset = static_cast<i8>(ic.get<u8>());

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
    u8 operand = ic.get<u8>();

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
    u8 operand = ic.get<u8>();

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
        i8 offset = static_cast<i8>(ic.get<u8>()); // Pobierz offset skoku (może być dodatni lub ujemny)
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
    u16 address = ic.get<u16>(); // Adres jest przechowywany w value, może to być u16

    // Zapisz wartość akumulatora do pamięci
    ic.write(address, ic.regs->A);
}

void opcodes::STY(InstructionContext ic) {
    // Pobierz wartość adresu, pod który należy zapisać zawartość rejestru Y
    u16 address = ic.get<u16>(); // Adres jest przechowywany w value

    // Zapisz wartość rejestru Y do pamięci
    ic.write(address, ic.regs->Y);
}

void opcodes::STX(InstructionContext ic) {
    u16 address = ic.get<u16>(); // Adres jest przechowywany w value

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
        ic.regs->PC += static_cast<i8>(ic.get<u8>()); // Przesunięcie jest w trybie relatywnym
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
    u8 value = ic.get<u8>();

    // Załaduj wartość do rejestru Y
    ic.regs->Y = value;

    // Ustaw flagi Z i N
    setZN(ic, ic.regs->Y);
}

void opcodes::LDA(InstructionContext ic) {
    // Pobierz wartość z pamięci lub natychmiastową
    u8 value = ic.get<u8>();

    // Załaduj wartość do akumulatora A
    ic.regs->A = value;

    // Ustaw flagi Z i N
    setZN(ic, ic.regs->A);
}

void opcodes::LDX(InstructionContext ic) {
    // Pobierz wartość z pamięci lub natychmiastową
    u8 value = ic.get<u8>();

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
        i8 offset = static_cast<i8>(ic.get<u8>());

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
    u8 operand = ic.get<u8>();

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
    u8 operand = ic.get<u8>();

    // Wykonaj operację porównania
    u8 A = ic.regs->A;
    u16 result = static_cast<u16>(A) - static_cast<u16>(operand);

    // Ustaw flagę Carry, jeśli A >= operand
    ic.setStatus(StatusFlag::Carry, A >= operand);

    // Ustaw flagę Zero, jeśli wynik jest równy 0
    ic.setStatus(StatusFlag::Zero, (result & 0xFF) == 0);

    // Ustaw flagę Negative na podstawie najwyższego bitu wyniku
    ic.setStatus(StatusFlag::Negative, result & Bit7);
}

void opcodes::DEC(InstructionContext ic) {
    // Odczytaj wartość z pamięci pod wskazanym adresem
    u16 address = ic.get<u16>(); // Adres do modyfikacji
    u8 value = ic.read(address);

    // Zmniejsz wartość o 1
    u8 result = value - 1;

    // Zapisz wynik z powrotem do pamięci
    ic.write(address, result);

    setZN(ic, result);
}

void opcodes::CLB(InstructionContext ic) {
    // Pobierz adres i bit do wyczyszczenia
    u16 address = ic.get<u16>(); // Adres pamięci
    u8 bitToClear = ic.get<u8>(); // Numer bitu (0-7)

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
    u8 operand = ic.get<u8>(); // Pobierz wartość, z którą porównujemy

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
    u8 operand = ic.get<u8>();  // Pobierz operand (wartość z pamięci)

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
        u16 targetAddress = ic.regs->PC + ic.get<u8>(); // Operand to offset od obecnego PC

        // Ustaw PC na docelowy adres
        ic.regs->PC = targetAddress;
    }
}

void opcodes::SED(InstructionContext ic) {
    ic.setStatus(Decimal);
}

void opcodes::INC(InstructionContext ic) {
    // Odczytujemy operand (wartość, którą chcemy inkrementować)
    u8 operand = ic.read(ic.get<u16>());  // Odczytujemy wartość z pamięci pod adresem zawartym w value

    // Zwiększ wartość o 1
    operand++;

    // Zapisz wynik z powrotem do pamięci
    ic.write(ic.get<u16>(), operand);

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
        u16 branchAddress = ic.regs->PC + ic.get<u8>();

        // Wykonaj skok
        ic.regs->PC = branchAddress;
    }
}

void opcodes::CLD(InstructionContext ic) {
    ic.clearStatus(Decimal);
}
