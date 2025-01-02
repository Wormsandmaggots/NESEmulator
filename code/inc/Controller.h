//
// Created by Radek on 30.12.2024.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <array>
#include <SDL_events.h>

#include "Types.h"


class Memory;

class Controller {
public:
    Controller(Memory* mem);
    void setButtonState(u8 button, bool pressed);
    void strobe(bool flag);
    u8 read();
    void updateFromSDL(const SDL_Event& event); // Nowa metoda do obsługi SDL2

private:
    Memory* mem;
    std::array<bool, 8> buttons = {}; // Stany przycisków
    uint8_t shiftRegister = 0;       // Rejestr przesuwający
    bool strobeFlag = false;         // Flaga strobu
};



#endif //CONTROLLER_H
