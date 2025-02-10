//
// Created by Radek on 13.01.2025.
//

#include "APU.h"
#include "Settings.h"
#include <cmath>
#include <SDL_audio.h>

#include "CPU.h"

using namespace apu;

Pulse::Pulse(uint8_t channel) {
    enabled = false;
    this->channel = channel;
    lengthEnabled = false;
    lengthValue = 0;
    timerPeriod = 0;
    timerValue = 0;
    dutyMode = 0;
    dutyValue = 0;
    sweepReload = false;
    sweepEnabled = false;
    sweepNegate = false;
    sweepShift = 0;
    sweepPeriod = 0;
    sweepValue = 0;
    envelopeEnabled = false;
    envelopeLoop = false;
    envelopeStart = false;
    envelopePeriod = 0;
    envelopeValue = 0;
    envelopeVolume = 0;
    constantVolume = 0;
}

void Pulse::writeControl(uint8_t value) {
    dutyMode = (value >> 6) & 3;
    lengthEnabled = ((value >> 5) & 1) == 0;
    envelopeLoop = ((value >> 5) & 1) == 1;
    envelopeEnabled = ((value >> 4) & 1) == 0;
    envelopePeriod = value & 15;
    constantVolume = value & 15;
    envelopeStart = true;
}

void Pulse::writeSweep(uint8_t value) {
    sweepEnabled = ((value >> 7) & 1) == 1;
    sweepPeriod = ((value >> 4) & 7) + 1;
    sweepNegate = ((value >> 3) & 1) == 1;
    sweepShift = value & 7;
    sweepReload = true;
}

void Pulse::writeTimerLow(uint8_t value) {
    timerPeriod = (timerPeriod & 0xff00) | (uint16_t)value;
}

void Pulse::writeTimerHigh(uint8_t value) {
    lengthValue = lengthTable[value >> 3];
    timerPeriod = (timerPeriod & 0xff) | ((uint16_t)(value & 7) << 8);
    envelopeStart = true;
    dutyValue = 0;
}

void Pulse::stepTimer() {
    if (timerValue == 0)
    {
        timerValue = timerPeriod;
        dutyValue = (dutyValue + 1) % 8;
    }
    else
    {
        timerValue--;
    }
}

void Pulse::stepSweep() {
    if (sweepReload)
    {
        if (sweepEnabled && sweepValue == 0)
        {
            sweep();
        }
        sweepValue = sweepPeriod;
        sweepReload = false;
    }
    else if (sweepValue > 0)
    {
        sweepValue--;
    }
    else
    {
        if (sweepEnabled)
        {
            sweep();
        }
        sweepValue = sweepPeriod;
    }
}

void Pulse::stepEnvelope() {
    if (envelopeStart)
    {
        envelopeVolume = 15;
        envelopeValue = envelopePeriod;
        envelopeStart = false;
    }
    else if (envelopeValue > 0)
    {
        envelopeValue--;
    }
    else
    {
        if (envelopeVolume > 0)
        {
            envelopeVolume--;
        }
        else if (envelopeLoop)
        {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}

void Pulse::stepLength() {
    if (lengthEnabled && lengthValue > 0)
    {
        lengthValue--;
    }
}

void Pulse::sweep() {
    uint16_t delta = timerPeriod >> sweepShift;
    if (sweepNegate)
    {
        timerPeriod -= delta;
        if (channel == 1)
        {
            timerPeriod--;
        }
    }
    else
    {
        timerPeriod += delta;
    }
}

u8 Pulse::out() const {
    if (!enabled)
    {
        return 0;
    }

    if (lengthValue == 0)
    {
        return 0;
    }

    if (dutyTable[dutyMode][dutyValue] == 0)
    {
        return 0;
    }

    if (timerPeriod < 8 || timerPeriod > 0x7ff)
    {
        return 0;
    }

    if (envelopeEnabled)
    {
        return envelopeVolume;
    }

    return constantVolume;
}

Triangle::Triangle() {
    enabled = false;
    lengthEnabled = false;
    lengthValue = 0;
    timerPeriod = 0;
    dutyValue = 0;
    counterPeriod = 0;
    counterValue = 0;
    counterReload = false;
}

void Triangle::writeControl(uint8_t value) {
    lengthEnabled = ((value >> 7) & 1) == 0;
    counterPeriod = value & 0x7f;
}

void Triangle::writeTimerLow(uint8_t value) {
    timerPeriod = (timerPeriod & 0xff00) | (uint16_t)value;
}

void Triangle::writeTimerHigh(uint8_t value) {
    lengthValue = lengthTable[value >> 3];
    timerPeriod = (timerPeriod & 0x00ff) | ((uint16_t)(value & 7) << 8);
    timerValue = timerPeriod;
    counterReload = true;
}

void Triangle::stepTimer() {
    if (timerValue == 0)
    {
        timerValue = timerPeriod;
        if (lengthValue > 0 && counterValue > 0)
        {
            dutyValue = (dutyValue + 1) % 32;
        }
    }
    else
    {
        timerValue--;
    }
}

void Triangle::stepLength() {
    if (lengthEnabled && lengthValue > 0)
    {
        lengthValue--;
    }
}

void Triangle::stepCounter() {
    if (counterReload)
    {
        counterValue = counterPeriod;
    }
    else if (counterValue > 0)
    {
        counterValue--;
    }
    if (lengthEnabled)
    {
        counterReload = false;
    }
}

u8 Triangle::out() const {
    if (!enabled)
    {
        return 0;
    }

    if (lengthValue == 0)
    {
        return 0;
    }

    if (counterValue == 0)
    {
        return 0;
    }

    return triangleTable[dutyValue];
}

APU::APU(Memory *sharedMemory) {
    sharedMemory->beforeWrite.push_back([this](u16 addr, u8 val) -> bool {
        switch (addr) {
            case 0x4000:
                pulse1.writeControl(val);
                break;
            case 0x4001:
                pulse1.writeSweep(val);
                break;
            case 0x4002:
                pulse1.writeTimerLow(val);
                break;
            case 0x4003:
                pulse1.writeTimerHigh(val);
                break;
            case 0x4004:
                pulse2.writeControl(val);
                break;
            case 0x4005:
                pulse2.writeSweep(val);
                break;
            case 0x4006:
                pulse2.writeTimerLow(val);
                break;
            case 0x4007:
                pulse2.writeTimerHigh(val);
                break;
            case 0x4008:
                triangle.writeControl(val);
                break;
            case 0x400A:
                triangle.writeTimerLow(val);
                break;
            case 0x400B:
                triangle.writeTimerHigh(val);
                break;
            case 0x4015:
                writeControl(val);
                break;
            case 0x4017:
                frameCounter.stopIRQ = val & Bit6;
                frameCounter.mode4Step = val & Bit7;
                break;
        }

        return true;
    });
}

void APU::step(nes_cycle_t count) {
    while(master_cycle < count) {
        stepAPU(nes_apu_cycle_t(1));

        //frame counter/ sequencer
        u8 previousStepCounter = frameCounter.currentTick;

        for (auto i : stepTable) {
            if(apu_cycle.count() == i) {
                frameCounter.currentTick++;
                break;
            }
        }

        if(previousStepCounter != frameCounter.currentTick) {
            sequencer();
        }

        if(sample_cycle.count() > 20) {
            sampleBuffer.push_back(getSample());
            sample_cycle = nes_apu_cycle_t(0);
        }
    }
}

std::vector<u8>* APU::getSampleBuffer() {
    return &sampleBuffer;
}

void APU::clearSampleBuffer() {
    sampleBuffer.clear();
}

void APU::stepAPU(nes_apu_cycle_t count) {
    master_cycle += count;
    apu_cycle += count;
    sample_cycle += count;

    pulse1.stepTimer();
    pulse2.stepTimer();
    triangle.stepTimer();

    if(apu_cycle.count() > stepTable[3]) {
        apu_cycle = nes_apu_cycle_t(0);
        frameCounter.currentTick = 0;
    }
}

void APU::sequencer() {
    switch (frameCounter.currentTick) {
        case 0:
            quarterFrame();
            break;
        case 1:
            quarterFrame();
            halfFrame();
            break;
        case 2:
            quarterFrame();
            break;
        case 3:
            quarterFrame();
            halfFrame();
            invokeIRQ();
            break;
    }
}

void APU::invokeIRQ() const {
    if(!frameCounter.stopIRQ)
        CPU::setIRQ();
}

void APU::quarterFrame() {
    pulse1.stepLength();
    pulse2.stepLength();
    triangle.stepLength();
}

void APU::halfFrame() {
    pulse1.stepEnvelope();
    pulse2.stepEnvelope();
    triangle.stepCounter();

    pulse1.stepSweep();
    pulse2.stepSweep();
}

u8 APU::getSample() {
    double pulseOut = 0.00752 * (pulse1.out() + pulse2.out());
    double tndOut = 0.00851 * triangle.out();
    double returnValue = floor(255.0 * (pulseOut + tndOut));

    WARNLOG(std::to_string(static_cast<u8>(returnValue)));
    return static_cast<u8>(returnValue);
}

void APU::writeControl(uint8_t val) {
    pulse1.enabled = (val & 1) == 1;
    pulse2.enabled = (val & 2) == 2;
    triangle.enabled = (val & 4) == 4;
    if (!pulse1.enabled)
    {
        pulse1.lengthValue = 0;
    }
    if (!pulse2.enabled)
    {
        pulse2.lengthValue = 0;
    }
    if (!triangle.enabled)
    {
        triangle.lengthValue = 0;
    }
}
