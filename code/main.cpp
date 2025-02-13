#include <iostream>
#include <SDL.h>
#include "Cartridge.h"
#include "Controller.h"
#include "CPU.h"
#include "Logger.h"
#include "Opcodes.h"
#include "PPU.h"
#include "NESHelpers.h"
#include "APU.h"

using namespace std;

void audioCallback(void* userdata, u8* stream, int len) {
    APU* apu = static_cast<APU*>(userdata);
    if(apu == null)
        return;

    vector<u8>* sampleBuffer = apu->getSampleBuffer();
    if(!sampleBuffer) return;

    if (sampleBuffer->size() > len) {
        memcpy(stream, sampleBuffer->data(), len);
        sampleBuffer->erase(sampleBuffer->begin(), sampleBuffer->begin() + len);
    }
}

int main(int argc, char* argv[])
{
    CPU cpu;
    cpu.init();

    assert(argc > 1);

    Cartridge cartridge(argv[1]);

    assert(cartridge.load());
    cartridge.loadToMemory(cpu.getMemory());
    cpu.reset();

    PPU ppu(cpu.getMemory());
    APU apu(cpu.getMemory());

    cartridge.loadToVRam(&ppu);

    Controller p1(cpu.getMemory(), input::p1, input::firstPlayerKeys);
    Controller p2(cpu.getMemory(), input::p2, input::secondPlayerKeys);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL Cmake Template",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                              resolution.x, resolution.y);
    if (!texture) {
        SDL_Log("Unable to create texture! SDL_Error: %s", SDL_GetError());
        return -1;
    }

    SDL_AudioSpec desiredSpec, obtainedSpec;
    desiredSpec.freq = NES_CLOCK_HZ / 120;
    desiredSpec.format = AUDIO_U8;
    desiredSpec.channels = 1;
    desiredSpec.samples = 4096;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = &apu;

    SDL_AudioDeviceID device;
    device = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, 0), 0, &desiredSpec, &obtainedSpec, 0);
    cout << "play audio on device n°" << device << endl;

    SDL_PauseAudioDevice(device, 0);

    bool isOn = true;
    u64 prev_counter = SDL_GetPerformanceCounter();
    u64 count_per_second = SDL_GetPerformanceFrequency();

    nes_cycle_t _master_cycle = nes_cycle_t(0);

    while (isOn)
    {
        // input handling
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isOn = false;
            }
            if (event.type == SDL_WINDOWEVENT
                && event.window.event == SDL_WINDOWEVENT_CLOSE
                && event.window.windowID == SDL_GetWindowID(window))
            {
                isOn = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                isOn = false;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
                INFOLOG(cpu.getRegisters()->toString());
            }
        }

        u64 cur_counter = SDL_GetPerformanceCounter();

        u64 delta_ticks = cur_counter - prev_counter;
        prev_counter = cur_counter;
        if (delta_ticks == 0)
            delta_ticks = 1;

        auto cpu_cycles = ms_to_nes_cycle((double)delta_ticks * 1000 / count_per_second);

        if (cpu_cycles > nes_cycle_t(NES_CLOCK_HZ))
            cpu_cycles = nes_cycle_t(NES_CLOCK_HZ);

        for (nes_cycle_t i = nes_cycle_t(0); i < cpu_cycles; ++i) {
            _master_cycle += nes_cycle_t(1);
            cpu.step(_master_cycle);
            ppu.step(_master_cycle);
            apu.step(_master_cycle);
        }

        SDL_UpdateTexture(texture, null, ppu.getFrame().data(), resolution.x * sizeof(u32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, null, null);
        SDL_RenderPresent(renderer);
    }

    SDL_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}