//
// Created by Radek on 01.11.2024.
//

#include "Opcodes.h"

#include "Logger.h"
#include "LogMessages.h"
#include "NESHelpers.h"

using namespace opcodes;

void opcodes::BRKimpl(InstructionContext<void*> ic) {
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

void opcodes::ORAindX(InstructionContext<> ic) {
    u8 value = ic.read(ic.regs->X + ic.value.value()); // Dodajemy wartość X do adresu

    // Wykonaj operację OR z akumulatorem (rejestr A)
    ic.regs->A |= value;

    // Ustawienie flagi Zero (jeśli wynik jest 0)
    ic.setStatus(StatusFlag::Zero, ic.regs->A == 0);

    // Usawienie flagi Negative (jeśli bit 7 w akumulatorze jest ustawiony)
    ic.setStatus(StatusFlag::Negative, ic.regs->A & 0x80);
}

void opcodes::Illegal(InstructionContext<> ic) {
    ERRORLOG(error::unimplementedOpcode);
}
