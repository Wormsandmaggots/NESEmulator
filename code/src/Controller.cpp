//
// Created by Radek on 30.12.2024.
//

#include "Controller.h"
#include "Memory.h"
#include "NESHelpers.h"

Controller::Controller(Memory *mem, uint16_t relatedMemoryAddress, const SDL_Keycode* keys) {
    sharedMemory = mem;

    controllerInputMemoryAddress = relatedMemoryAddress;

    for(int i = 0; i < 8; i++) {
        keymap.insert({SDL_GetScancodeFromKey(keys[i]), false});
    }

    mem->beforeWrite.push_back([this](u16 addr, u8& val) -> bool {
        if(addr == controllerInputMemoryAddress) {
            bool prevStrobe = strobeFlag;
            strobeFlag = (val & Bit1);
            if(!prevStrobe && !strobeFlag) {
                fetchInput();
            }

            return false;
        }

        return true;
    });

    mem->beforeRead.push_back([this](u16 addr) -> std::optional<u8> {
        if(addr == controllerInputMemoryAddress) {
            if(strobeFlag)
                getButtonState();

            return 0x40 | (keysValue >> (7 - buttonID++)) & Bit0;
        }

        return std::nullopt;
    });
}

void Controller::setButtonState(SDL_Keycode button, bool pressed) {
    if(keymap.contains(SDL_GetScancodeFromKey(button))) {
        keymap[SDL_GetScancodeFromKey(button)] = pressed;
    }
}

void Controller::setButtonState(u8 val) {
    u8 shift = 1;

    for (auto& key: keymap) {
        key.second = val & shift;
        shift <<= 1;
    }
}

u8 Controller::getButtonState() {
    keysValue = 0;
    u8 shift = 1;

    for(auto& key: keymap) {
        if(key.second)
            keysValue |= shift;

        shift <<= 1;
    }

    return keysValue;
}

void Controller::strobe(bool flag) {
    strobeFlag = flag;
    if (strobeFlag) {
        keysValue = 0;
        u8 shift = 0;
        for(auto& key : keymap) {
            if(key.second)
                keysValue |= (1 << shift);

            shift++;
        }
    }
}

uint8_t Controller::read() {
    uint8_t bit = keysValue & 1;
    keysValue >>= 1;
    return bit;
}

void Controller::updateFromSDL(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto key = event.key.keysym.sym;

        setButtonState(key, event.type == SDL_KEYDOWN);
    }
}

void Controller::fetchInput() {
    auto keyboard = SDL_GetKeyboardState(null);

    for(auto& key: keymap) {
        if(keyboard[key.first]) {
            key.second = true;
        }
    }

    buttonID = 0;
}
