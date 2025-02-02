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

#include "Logger.h"


class PPU;
using namespace std::chrono;

#define NES_CLOCK_HZ (21477272ll / 4)

typedef duration<i64, std::ratio<1, 1>> nes_cycle_t;
typedef duration<i64, std::ratio<1, 1>> nes_ppu_cycle_t;
typedef duration<i64, std::ratio<3, 1>> nes_cpu_cycle_t;
typedef duration<i64, std::ratio<6, 1>> nes_apu_cycle_t;

nes_cycle_t ms_to_nes_cycle(double ms);

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
        return (r << 24) | (g << 16) | (b << 8);
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

        std::string toString() const;
    };
}

namespace opcodes {
    bool isBranchInstruction(u8 opcode);
    bool branchConditionMet(u8 opcode, const cpu::Registers& regs);
    bool hasPageCrossed(u16 baseAddr, i8 offset);
    bool u16AddressingMode(cpu::AddressingMode addressingMode);
    bool i8AddressingMode(cpu::AddressingMode addressingMode);

    u16 mergeToU16(const u8 low, const u8 high);
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

        bool spriteOverflow = false;
        bool vblank = false;
        bool sprite0Hit = false;

        std::vector<u8> oam;

        bool showBackground() const;
        bool showSprites() const;
        u16 spritePatternTableAddress() const;
        u16 backgroundPatternTableAddress() const;
        u16 nametableAddress() const;
        u8 ppuAddressIncValue() const;
        bool vblankNmi() const;
        void setSpriteOverflow(bool val);
        bool getSpriteOverflow() const;
        bool use8x16Sprites() const;
        bool getSprite0Hit() const;
        void setSprite0Hit(bool val);
        void setVblankFlag(bool val);
        bool getVblankFlag() const;

        void writeLatch(u8 val);
        void writePPUSCROLL(u8 val);
        void writePPUCTRL(u8 val);
        void writePPUAADDR(u8 val);
        void writePPUMASK(u8 val);
        void writeOAMADDR(u8 val);
        void writeOAMDATA(u8 val);
        void writePPUData(u8 val, PPU* ppu);
        void writeOAMDMA(u8 val);

        u8 readPPUSTATUS();
        u8 readOAMDATA();
        u8 readPPUDATA(PPU* ppu);
    };
}

namespace input {

    enum class Button : u8 {
        right = 0x1,
        left = 0x2,
        down = 0x4,
        up = 0x8,
        select = 0x10,
        start = 0x20,
        b = 0x40,
        a = 0x80
    };
    constexpr u16 p1 = 0x4016;
    constexpr u16 p2 = 0x4017;

    constexpr SDL_Scancode firstPlayerKeys[] = {
        SDL_SCANCODE_D,
        SDL_SCANCODE_A,
        SDL_SCANCODE_S,
        SDL_SCANCODE_W,
        SDL_SCANCODE_LSHIFT,
        SDL_SCANCODE_LCTRL,
        SDL_SCANCODE_G,
        SDL_SCANCODE_H
    };

    constexpr SDL_Scancode secondPlayerKeys[] = {
        SDL_SCANCODE_RIGHT,
        SDL_SCANCODE_LEFT,
        SDL_SCANCODE_DOWN,
        SDL_SCANCODE_UP,
        SDL_SCANCODE_RSHIFT,
        SDL_SCANCODE_KP_ENTER,
        SDL_SCANCODE_K,
        SDL_SCANCODE_L
    };
}

namespace apu {

    constexpr u8 lengthTable[] = {
        10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
        12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };

    constexpr u8 dutyTable[][8] = {
        {0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0},
        {1, 0, 0, 1, 1, 1, 1, 1}
    };

    constexpr u8 triangleTable[] = {
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };

    constexpr u16 noiseTable[] = {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
    };

    constexpr u32 stepTable[] = {
        7457, 14913, 22371, 29829
    };

    struct FrameCounter {
        bool mode4Step = true;
        bool stopIRQ = false;
        u8 currentTick = 0;
    };

    // struct Pulse {
    //     //0x4000 or 0x4004
    //     bool enabled = false;
    //     u8 channel = 0;
    //     u8 dutyCycle = 0;
    //     bool lengthCounterHalt = false;
    //     bool constant = false;
    //     u8 envelopeDividerPeriod = 0;
    //
    //     //0x4001 or 0x4005
    //     bool enabledSweep = false;
    //     u8 sweepDividerPeriod = 0;
    //     bool negate = false;
    //     u8 shiftCount = 0;
    //     u8 envelopeVolume = 0;
    //
    //     //0x4002 or 0x4006
    //     u8 timerLow = 0;
    //
    //     //0x4003 or 0x4007
    //     u8 lengthCounterLoad = 0;
    //     u8 timerHigh = 0;
    //
    //     u32 phaseAccumulator = 0;
    //     u16 timerCounter = 0;
    //     u8 constantVolume = 0;
    //
    //     void writeSweep(u8 val);
    //     void writeTimerLow(u8 val);
    //     void writeDutyAndEnvelope(u8 val);
    //     void writeLengthCounterAndTimerHigh(u8 val);
    //     void stepSweep();
    //     void stepLengthCounter();
    //     void stepEnvelope();
    //     void stepTimer();
    //     uint8_t getSample();
    // };
    //
    // struct Triangle {
    //     bool enabled = false;
    //     uint16_t timerPeriod = 0;
    //     uint16_t timer = 0;
    //     uint8_t lengthCounter = 0;
    //     uint8_t linearCounter = 0;
    //     uint8_t linearCounterReload = 0;
    //     bool controlFlag = false;
    //     bool haltFlag = false;
    //     uint8_t sequenceIndex = 0;
    //
    //
    //     void writeControl(uint8_t value) {
    //         controlFlag = (value & 0x80) != 0;
    //         linearCounterReload = value & 0x7F;
    //     }
    //
    //     void writeTimerLow(uint8_t value) {
    //         timerPeriod = (timerPeriod & 0xFF00) | value;
    //     }
    //
    //     void writeTimerHigh(uint8_t value) {
    //         timerPeriod = (timerPeriod & 0x00FF) | ((value & 0x07) << 8);
    //         lengthCounter = lengthTable[value >> 3];
    //         linearCounter = linearCounterReload;
    //         haltFlag = true;
    //     }
    //
    //     void stepTimer() {
    //         if (timer > 0) {
    //             --timer;
    //         } else {
    //             timer = timerPeriod;
    //             if (lengthCounter > 0 && linearCounter > 0) {
    //                 sequenceIndex = (sequenceIndex + 1) % 32;
    //             }
    //         }
    //     }
    //
    //     void stepLinearCounter() {
    //         if (haltFlag) {
    //             linearCounter = linearCounterReload;
    //         } else if (linearCounter > 0) {
    //             --linearCounter;
    //         }
    //         if (!controlFlag) {
    //             haltFlag = false;
    //         }
    //     }
    //
    //     void stepLengthCounter() {
    //         if (!controlFlag && lengthCounter > 0) {
    //             --lengthCounter;
    //         }
    //     }
    //
    //     uint8_t getSample() {
    //         if (!enabled || lengthCounter == 0 || linearCounter == 0) {
    //             return 0;
    //         }
    //         return triangleTable[sequenceIndex];
    //     }
    // };

}

#endif //NESHELPERS_H
