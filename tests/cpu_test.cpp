#include <gtest/gtest.h>

#include "Cartridge.h"
#include "CPU.h"

TEST(CPUTest, nestest) {
    CPU cpu;
    cpu.init();  // Inicjalizacja procesora

    Cartridge cartridge("nestest.nes");
    cartridge.loadToMemory(cpu.getMemory());
    cpu.reset();
}