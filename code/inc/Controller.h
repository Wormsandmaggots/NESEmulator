//
// Created by Radek on 30.12.2024.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL_events.h>
#include <vector>

#include "Types.h"


class Memory;

class Controller {
public:
    Controller(Memory* mem, u16 relatedMemoryAddress, const SDL_Scancode* keys);
    void setButtonState(SDL_Keycode button, bool pressed);
    void setButtonState(u8 val);
    u8 getButtonState();
    void updateFromSDL(const SDL_Event& event); // Nowa metoda do obsługi SDL2

private:
    Memory* sharedMemory = null;
    std::vector<std::pair<SDL_Scancode, bool>> keymap;
    u16 controllerInputMemoryAddress = 0;
    u8 keysValue = 0;
    u8 buttonID = 0;

    void fetchInput();
};


#endif //CONTROLLER_H
