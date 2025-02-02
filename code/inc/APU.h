//
// Created by Radek on 13.01.2025.
//

#ifndef APU_H
#define APU_H
#include "NESHelpers.h"

using namespace apu;

class Pulse {
public:
    friend class APU;

    explicit Pulse(u8 channel);
    void writeControl(u8);
    void writeSweep(u8);
    void writeTimerLow(u8);
    void writeTimerHigh(u8);
    void stepTimer();
    void stepSweep();
    void stepEnvelope();
    void stepLength();
    void sweep();

    u8 out() const;

private:
    bool enabled;
    uint8_t channel;
    bool lengthEnabled;
    uint8_t lengthValue;
    uint16_t timerPeriod;
    uint16_t timerValue;
    uint8_t dutyMode;
    uint8_t dutyValue;
    bool sweepReload;
    bool sweepEnabled;
    bool sweepNegate;
    uint8_t sweepShift;
    uint8_t sweepPeriod;
    uint8_t sweepValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopePeriod;
    uint8_t envelopeValue;
    uint8_t envelopeVolume;
    uint8_t constantVolume;

};

class Noise {
public:
    friend class APU;

    Noise();

    void writeControl(u8);
    void writePeriod(u8);
    void writeLength(u8);
    void stepTimer();
    void stepEnvelope();
    void stepLength();

    u8 out() const;

private:
    bool enabled;
    bool mode;
    uint16_t shiftRegister;
    bool lengthEnabled;
    uint8_t lengthValue;
    uint16_t timerPeriod;
    uint16_t timerValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopePeriod;
    uint8_t envelopeValue;
    uint8_t envelopeVolume;
    uint8_t constantVolume;
};

class Triangle {
public:
    friend class APU;

    Triangle();

    void writeControl(u8);
    void writeTimerLow(u8);
    void writeTimerHigh(u8);
    void stepTimer();
    void stepLength();
    void stepCounter();

    u8 out() const;

private:
    bool enabled;
    bool lengthEnabled;
    uint8_t lengthValue;
    uint16_t timerPeriod;
    uint16_t timerValue;
    uint8_t dutyValue;
    uint8_t counterPeriod;
    uint8_t counterValue;
    bool counterReload;
};


class APU {
public:
    explicit APU(Memory* sharedMemory);
    void step(nes_cycle_t count);

    std::vector<u8>* getSampleBuffer();
    void clearSampleBuffer();
    bool getLocked();
    std::mutex& getMutex();

private:
    FrameCounter frameCounter;

    nes_cycle_t master_cycle = nes_cycle_t(0);
    nes_apu_cycle_t apu_cycle = nes_apu_cycle_t(0);
    nes_apu_cycle_t sample_cycle = nes_apu_cycle_t(0);

    Pulse pulse1 = Pulse(1);
    Pulse pulse2 = Pulse(2);
    Triangle triangle;

    std::vector<u8> sampleBuffer;

    std::mutex mutex;

    bool locked = false;

    void stepAPU(nes_apu_cycle_t);
    void sequencer();
    void invokeIRQ() const;
    void quarterFrame();
    void halfFrame();
    u8 getSample();

    void writeControl(u8 val);
// public:
//      explicit APU(Memory* sharedMemory);
//      void step(nes_cycle_t);
//      void out(u8* buff, u32 len);
//      void audioCallback(void* userdata, uint8_t* buffer, int len);
//
// private:
//      u8 frame = 0;
//
//      nes_cycle_t cycles;
//     nes_cycle_t apuCycles;
//      u8 audioBuffer[audioBufferSize] = {0};
//      u64 audioBufferLength = 0;
//     void stepAPU(nes_apu_cycle_t);
//     void sequencer();
//     void invokeIRQ() const;
//     void quarterFrame();
//     void halfFrame();
//
//      Pulse pulse1 = Pulse(1);
//      Pulse pulse2 = Pulse(2);
//      Noise noise;
//      Triangle triangle;
//
//     nes_cycle_t master_cycle = nes_cycle_t(0);
//     nes_apu_cycle_t apu_cycle = nes_apu_cycle_t(0);
//     nes_apu_cycle_t sample_cycle = nes_apu_cycle_t(0);
//
//      void writeControl(u8 val);
//      void stepLength();
//      void stepSweep();
//      void stepEnvelope();
//
//      u8 getSample();
};


#endif //APU_H
