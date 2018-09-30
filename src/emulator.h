#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include "types.h"
#include "memory_manager.h"
#include "cpu.h"

class Emulator
{
public:
    Emulator();
    ~Emulator();

    void run();

    /* Load a file that represents a gameboy cartridge into memory. */
    void loadCartridge(std::string fileName);

private:
    MemoryManager* mem;
    Cpu* cpu;
};

#endif /* EMULATOR_H */
