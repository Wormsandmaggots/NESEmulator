#include <iostream>
#define MINIAUDIO_IMPLEMENTATION
#include <SDL.h>
#include "Cartridge.h"
#include "Controller.h"
#include "CPU.h"
#include "Logger.h"
#include "Opcodes.h"
#include "PPU.h"
#include "NESHelpers.h"
#include "APU.h"
#include "miniaudio.h"

using namespace std;

void testDevices(){
    for (uint8_t i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        printf("Audio driver %d: %s\n", i, SDL_GetAudioDriver(i));
    }

    cout << "current audio driver is " << SDL_GetCurrentAudioDriver() << endl;

    int nbDevice = SDL_GetNumAudioDevices(0);

    for(int i = 0; i < nbDevice; ++i){
        cout << "device n°" << i << ": ";
        cout << SDL_GetAudioDeviceName(i, 0) << endl;
    }
}

void audioCallback2(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    APU* apu = (APU*)pDevice->pUserData;
    if (!apu) return;

    vector<uint8_t>* sampleBuffer;// = apu->getSampleBuffer();
    if(!sampleBuffer) return;

    uint8_t* outputBuffer = (uint8_t*)pOutput;
    size_t bufferSize = sampleBuffer->size();

    // Sprawdź, czy mamy wystarczającą liczbę próbek
    if (bufferSize >= frameCount) {
        memcpy(outputBuffer, sampleBuffer->data(), frameCount);
        sampleBuffer->erase(sampleBuffer->begin(), sampleBuffer->begin() + frameCount);
    } else {
        // Jeśli nie mamy wystarczających próbek, wypełniamy ciszą
        memset(outputBuffer, 0, frameCount); // 128 to środek zakresu dla AUDIO_U8
    }
}

void audioCallback(void* userdata, u8* stream, int len) {
    APU* apu = static_cast<APU*>(userdata);
    if(apu == null)
        return;

    //apu->out(stream, len);
    vector<uint8_t>* sampleBuffer = apu->getSampleBuffer();
    if(!sampleBuffer) return;

    // Jeśli w buforze jest wystarczająco próbek, kopiujemy je do strumienia audio
    if (sampleBuffer->size() >= len) {
        memcpy(stream, sampleBuffer->data(), len);
        sampleBuffer->erase(sampleBuffer->begin(), sampleBuffer->begin() + len);
    } else {
        // Jeśli nie ma wystarczająco próbek, wypełniamy strumień ciszą (0)
        //memset(stream, 128, len);
        // memcpy(stream, apu->getSampleBuffer()->data(), apu->getSampleBuffer()->size());
        // memset(stream + apu->getSampleBuffer()->size(), 0, len - apu->getSampleBuffer()->size());
        // sampleBuffer->clear();
    }

    //sampleBuffer.clear();
}

int main(int argc, char* argv[])
{
    CPU cpu;
    cpu.init();

    Cartridge cartridge(R"(D:\Projects\roms\mariopal.nes)");

    cartridge.load();
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

    // Stwórz teksturę dla bufora ramki
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                              resolution.x, resolution.y);
    if (!texture) {
        SDL_Log("Unable to create texture! SDL_Error: %s", SDL_GetError());
        return -1;
    }

    // testDevices();
    //
    // Inicjalizacja SDL Audio
    SDL_AudioSpec desiredSpec, obtainedSpec;
    desiredSpec.freq = 44100; // Częstotliwość próbkowania
    desiredSpec.format = AUDIO_U8; // Format próbek (8-bit unsigned)
    desiredSpec.channels = 1; // Mono
    desiredSpec.samples = 1024; // Rozmiar bufora audio
    desiredSpec.callback = audioCallback; // Funkcja callback
    desiredSpec.userdata = &apu; // Przekazanie wskaźnika do APU

    SDL_AudioDeviceID device;
    device = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, 0), 0, &desiredSpec, &obtainedSpec, 0);
    cout << "play audio on device n°" << device << endl;

    SDL_PauseAudioDevice(device, 0);
    //SDL_PauseAudio(0);

    // ma_device_config config = ma_device_config_init(ma_device_type_playback);
    // config.playback.format   = ma_format_s16; // AUDIO_U8 (unsigned 8-bit PCM)
    // config.playback.channels = 1; // Mon
    // config.sampleRate        = 48000; // 44.1kHz
    // config.dataCallback      = audioCallback2;
    // config.pUserData         = &apu;
    //
    // ma_device device;
    // if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
    //     cout << "Nie udało się uruchomić miniaudio!" << endl;
    //     return -1;
    // }
    //
    // // Uruchamiamy odtwarzanie
    // ma_device_start(&device);

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

    //SDL_CloseAudio(); // Zamknij urządzenie audio
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}