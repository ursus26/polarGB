#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include "types.h"
#include "mmu.h"
#include "cpu.h"
#include "video.h"


class Emulator
{
public:
    Emulator();
    ~Emulator();

    void run();

    /* Load a file that represents a gameboy cartridge into memory. */
    void loadCartridge(std::string fileName);

private:
    Mmu* mmu;
    Cpu* cpu;
    Video* video;
};

#endif /* EMULATOR_H */
