//
// Created by Radek on 06.11.2024.
//

#ifndef NESHELPERS_H
#define NESHELPERS_H

#include <iomanip>
#include <sstream>
#include <string>

#include "Memory.h"
#include "Types.h"
#include <chrono>
#include <SDL_keycode.h>


class PPU;
using namespace std::chrono;

#define NES_CLOCK_HZ (21477272ll / 4)

typedef duration<int64_t, std::ratio<1, 1>> nes_cycle_t;
typedef duration<int64_t, std::ratio<1, 1>> nes_ppu_cycle_t;
typedef duration<int64_t, std::ratio<3, 1>> nes_cpu_cycle_t;

static nes_cycle_t ms_to_nes_cycle(double ms)
{
    return nes_cycle_t(int64_t(NES_CLOCK_HZ / 1000 * ms));
}

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

struct Color {
    u8 r = 0;
    u8 g = 0;
    u8 b = 0;

    u32 makeu32() const {
        return (r << 16) | (g << 8) | b;
    }
};

namespace cpu {
    enum class AddressingMode : i8
    {
        Undefined = -1,

        ZeroPageIndexedX,
        ZeroPageIndexedY,
        AbsoluteIndexedX,
        AbsoluteIndexedY,
        IndexedIndirectX,
        IndexedIndirectY,
        Implicit,
        Accumulator,
        Immediate,
        ZeroPage,
        Absolute,
        Relative,
        Indirect
    };

    enum StatusFlag : u8
    {
        Carry = 0x01,
        Zero = 0x02,
        InterruptDisable = 0x04,
        Decimal = 0x08,
        Break = 0x10, // also B
        Always1 = 0x20,
        Overflow = 0x40,
        Negative = 0x80,
    };

    struct Registers
    {
        u8 A; //accumulator
        u8 X; //index
        u8 Y; //index
        u16 PC; //program counter
        u8 S; //stack pointer
        u8 P; //status register

        void setStatus(const StatusFlag flag){
            P |= flag;
        }

        void SetA(u8 val) {
            if(val == 32)
                A = val;
            A = val;
        }

        void setStatus(const StatusFlag flag, const bool condition){
            if(condition)
                setStatus(flag);
            else
                clearStatus(flag);
        }

        void clearStatus(const StatusFlag flag){
            P &= ~flag;
        }

        void resetStatus(){
            P = 0;
        }

        bool getStatus(const StatusFlag flag) const{
            return P & flag;
        }

        std::string toString() const {
            std::stringstream ss;

            ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2);
            ss << "A: " << static_cast<int>(A) << std::endl;
            ss << "X: " << static_cast<int>(X) << std::endl;
            ss << "Y: " << static_cast<int>(Y) << std::endl;
            ss << "PC: " << static_cast<int>(PC) << std::endl;
            ss << "S: " << static_cast<int>(S) << std::endl;
            ss << "Carry: " << getStatus(Carry) << std::endl;
            ss << "Zero: " << getStatus(Zero) << std::endl;
            ss << "Interrupt: " << getStatus(InterruptDisable) << std::endl;
            ss << "Decimal: " << getStatus(Decimal) << std::endl;
            ss << "Break: " << getStatus(Break) << std::endl;
            ss << "Always1: " << getStatus(Always1) << std::endl;
            ss << "Overflow: " << getStatus(Overflow) << std::endl;
            ss << "Negative: " << getStatus(Negative) << std::endl;

            return ss.str();
        }
    };
}

namespace opcodes {
    bool isBranchInstruction(u8 opcode);
    bool branchConditionMet(u8 opcode, const cpu::Registers& regs);
    bool hasPageCrossed(u16 baseAddr, i8 offset);
    bool u16AddressingMode(cpu::AddressingMode addressingMode);
    bool i8AddressingMode(cpu::AddressingMode addressingMode);

    inline u16 mergeToU16(const u8 low, const u8 high) {
        return static_cast<u16>(low) | (static_cast<u16>(high) << 8);
    }
}

namespace ppu {

    struct Sprite {
        u8 y = 0;
        u8 tileIndex = 0;
        u8 attribute = 0;
        u8 x = 0;
    };

    //https://www.nesdev.org/wiki/PPU_registers
    struct Registers {
        //u8 registers[8];       // Rejestry PPU
        u16 V = 0;                 // Rejestr adresu (current VRAM address)
        u16 T = 0;                 // Rejestr adresu tymczasowego
        u8 X = 0;                  // Przesunięcie przesuwu poziomego
        u8 W = 0;                   // Flaga przełączania adresu

        u8* PPUControl = null;
        u8* PPUMask = null;
        u8* PPUStatus = null;
        u8* OAMAddr = null;
        u8* OAMData = null;
        u8* PPUScroll = null;
        u8* PPUData = null;
        u8* OAMDMA = null;

        u8 yScroll = 0;
        u8 latch = 0;
        bool protect = false;
        bool maskOAMRead = false;

        std::vector<u8> oam;

        bool showBackground() const {
            return (*PPUMask) & Bit3;
        }

        bool showSprites() const {
            return (*PPUMask) & Bit4;
        }

        u16 spritePatternTableAddress() {
            return (*PPUControl) & Bit3 ? 0x1000 : 0x0000;
        }

        u16 backgroundPatternTableAddress() {
            return (*PPUControl) & Bit4 ? 0x1000 : 0x0000;
        }

        u16 nametableAddress() {
            return 0x2000 + uint16_t(*PPUControl & 0x3) * 0x400;
        }

        u8 ppuAddressIncValue() {
            return (*PPUControl) & Bit2 ? 32 : 1;
        }

        bool vblankNmi() {
            return (*PPUControl) & Bit7;
        }

        void setSpriteOverflow(bool val) {
            if(val)
                (*PPUStatus) |= Bit5;
            else
                (*PPUStatus) &= ~Bit5;
        }

        bool use8x16Sprites() {
            return (*PPUControl) & Bit5;
        }

        bool sprite0Hit() {
            return (*PPUStatus) & Bit6;
        }

        void setSprite0Hit(bool val) {
            if(val)
                (*PPUStatus) |= Bit6;
            else
                (*PPUStatus) &= ~Bit6;
        }

        void setVblankFlag(bool val) {
            if(val)
                (*PPUStatus) |= Bit7;
            else
                (*PPUStatus) &= ~Bit7;
        }

        void writeLatch(u8 val) {
            if(protect) return;

            latch = val;
        }

        void writePPUSCROLL(u8 val) {
            writeLatch(val);

            W = !W;
            if (W)
            {
                // first write
                T = (T & 0xffe0) | (val >> 3);
                X = val & 0x7;
            }
            else
            {
                // second write
                T = (T & 0xc1f) | (uint16_t(val & 0xf8) << 2) | (uint16_t(val & 0x7) << 12);
                yScroll = val;
            }

            (*PPUScroll) = val;
        }

        void writePPUCTRL(u8 val) {
            writeLatch(val);

            T = (T & 0xf3ff) | ((val & 0x3) << 10);

            (*PPUControl) = val;
        }

        void writePPUAADDR(u8 val) {
            writeLatch(val);

            W = !W;
            if (W)
            {
                // first write
                // note that both PPUADDR(2006) and PPUSCROLL (2005) share the same _temp_ppu_addr
                T = (T & 0x00ff) | (uint16_t(val & 0x3f) << 8);
            }
            else
            {
                //problem ppuaddres jest u8 a próbuje zapisać do niego u16 co jest źle
                // second write
                // note that both PPUADDR(2006) and PPUSCROLL (2005) share the same _temp_ppu_addr
                T = (T & 0xff00) | val;
                V = T;
            }
        }

        void writePPUMASK(u8 val) {
            writeLatch(val);
            (*PPUMask) = val;
        }

        void writeOAMADDR(u8 val) {
            writeLatch(val);
            (*OAMAddr) = val;
        }

        void writeOAMDATA(u8 val)
        {
            writeLatch(val);

            oam[(*OAMAddr)] = val;
            (*OAMAddr)++;
        }

        void writePPUData(u8 val, PPU* ppu);

        void writeOAMDMA(u8 val);

        u8 readPPUSTATUS() {
            u8 status = (*PPUStatus);

            if(!protect) {
                setVblankFlag(false);
                W = false;
                writeLatch(status);
            }

            return status;
        }

        u8 readOAMDATA() {
            if (maskOAMRead)
                return 0xff;

            uint8_t val = oam[(*OAMAddr)];
            writeLatch(val);
            return val;
        }

        u8 readPPUDATA(PPU* ppu);
    };
}

namespace input {

    enum class Button : u8 {
        left = 0x1,
        right = 0x2,
        down = 0x4,
        up = 0x8,
        select = 0x10,
        start = 0x20,
        b = 0x40,
        a = 0x80
    };
    constexpr u16 p1 = 0x4016;
    constexpr u16 p2 = 0x4017;

    constexpr SDL_Keycode firstPlayerKeys[] = {
        SDLK_a,
        SDLK_d,
        SDLK_s,
        SDLK_w,
        SDLK_LSHIFT,
        SDLK_LCTRL,
        SDLK_g,
        SDLK_h
    };

    constexpr SDL_Keycode secondPlayerKeys[] = {
        SDLK_RIGHT,
        SDLK_UP,
        SDLK_DOWN,
        SDLK_LEFT,
        SDLK_RSHIFT,
        SDLK_KP_ENTER,
        SDLK_k,
        SDLK_l
    };
}

#endif //NESHELPERS_H
