#include "emulator.h"
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

Emulator::Emulator()
{
    this->mmu = new Mmu();
    this->cpu = new Cpu(this->mmu);
}

Emulator::~Emulator()
{
    delete this->cpu;
    this->cpu = nullptr;

    delete this->mmu;
    this->mmu = nullptr;
}


void Emulator::run()
{
    /* First boot up the emulator with the correct values. */
    cpu->boot();

    // cpu->runNCycles(10);
    // cpu->runSingleFrame();

    /* Run the emulator. */
    cpu->run();
}


/**
 * Load a file that acts as a gameboy rom into the memory using the memory manager.
 */
void Emulator::loadCartridge(string fileName)
{
    this->mmu->loadRom(fileName);
}
