#include "emulator.h"
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

Emulator::Emulator()
{
    cout << "Hello Emulator!" << endl;

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

    // cpu->run(10);
    // cpu->runSingleFrame();

    while(true)
        cpu->runSingleFrame();
}


/**
 * Load a file that acts as a gameboy rom into the memory using the memory manager.
 */
void Emulator::loadCartridge(string fileName)
{
    this->mmu->loadRom(fileName);
}
