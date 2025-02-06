//
// Created by Radek on 30.12.2024.
//

#include "Controller.h"
#include "Memory.h"
#include "NESHelpers.h"

Controller::Controller(Memory *mem, uint16_t relatedMemoryAddress, const SDL_Scancode* keys) {
    sharedMemory = mem;

    controllerInputMemoryAddress = relatedMemoryAddress;

    for(int i = 0; i < 8; i++) {
        keymap.emplace_back(keys[i], false);
    }

    mem->beforeWrite.push_back([this](u16 addr, u8& val) -> bool {
        if(addr == controllerInputMemoryAddress) {
            if(val & Bit0) {
                fetchInput();
            }

            return false;
        }

        return true;
    });

    mem->beforeRead.push_back([this](u16 addr) -> std::optional<u8> {
        if(addr == controllerInputMemoryAddress) {
            buttonID %= 8;

            if(buttonID == 0) getButtonState();

            u8 val = (keysValue >> (7 - buttonID++) & Bit0);

            return val;
        }

        return std::nullopt;
    });
}

void Controller::setButtonState(SDL_Keycode button, bool pressed) {
    auto key = std::find_if(keymap.begin(), keymap.end(), [button](const auto& pair) {
        return pair.first == button;
    });

    if(key != keymap.end()) {
        key->second = pressed;
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

void Controller::updateFromSDL(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto key = event.key.keysym.sym;

        setButtonState(key, event.type == SDL_KEYDOWN);
    }
}

void Controller::fetchInput() {
    auto keyboard = SDL_GetKeyboardState(null);

    for(auto& key: keymap) {
        key.second = keyboard[key.first];
    }

    buttonID = 0;
}
