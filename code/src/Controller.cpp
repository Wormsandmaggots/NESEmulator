//
// Created by Radek on 30.12.2024.
//

#include "Controller.h"

#include <map>

void Controller::setButtonState(uint8_t button, bool pressed) {
    if (button < buttons.size()) {
        buttons[button] = pressed;
    }
}

void Controller::strobe(bool flag) {
    strobeFlag = flag;
    if (strobeFlag) {
        shiftRegister = 0;
        for (int i = 0; i < 8; i++) {
            if (buttons[i]) {
                shiftRegister |= (1 << i);
            }
        }
    }
}

uint8_t Controller::read() {
    uint8_t bit = shiftRegister & 1;
    shiftRegister >>= 1;
    return bit;
}

void Controller::updateFromSDL(const SDL_Event &event) {
    // Mapowanie klawiszy na przyciski kontrolera NES
    static const std::map<SDL_Keycode, uint8_t> keyMapping = {
        {SDLK_a, 0}, // A
        {SDLK_s, 1}, // B
        {SDLK_RETURN, 2}, // Select
        {SDLK_SPACE, 3}, // Start
        {SDLK_UP, 4}, // Up
        {SDLK_DOWN, 5}, // Down
        {SDLK_LEFT, 6}, // Left
        {SDLK_RIGHT, 7} // Right
    };

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto it = keyMapping.find(event.key.keysym.sym);
        if (it != keyMapping.end()) {
            setButtonState(it->second, event.type == SDL_KEYDOWN);
        }
    }
}
