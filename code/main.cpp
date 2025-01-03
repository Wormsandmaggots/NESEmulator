#include <iostream>

#include <SDL.h>
#include "Cartridge.h"
#include "CPU.h"
#include "Logger.h"
#include "Opcodes.h"
#include "PPU.h"

using namespace std;

int main(int argc, char* argv[])
{
    CPU cpu;
    cpu.init();

    Cartridge cartridge("mario.nes");

    cartridge.load();
    cartridge.loadToMemory(cpu.getMemory());
    cpu.reset();

    PPU ppu(cpu.getMemory());

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
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

    bool isOn = true;
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

        cpu.execute(cpu.getInstruction());
        ppu.update();

        SDL_UpdateTexture(texture, null, ppu.getFrameBuffer().data(), resolution.x * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, null, null);
        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}