#include "emulator.h"
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

Emulator::Emulator()
{
    cout << "Hello Emulator!" << endl;

    this->mem = new MemoryManager();
    this->cpu = new Cpu(this->mem);
}

Emulator::~Emulator()
{
    delete this->cpu;
    this->cpu = nullptr;

    delete this->mem;
    this->mem = nullptr;
}


void Emulator::run()
{
    cpu->run(25);
    // cpu->runSingleFrame();

    // while(true)
    //     cpu->runSingleFrame();
}


/**
 * Load a file that acts as a gameboy rom into the memory using the memory manager.
 */
void Emulator::loadCartridge(string fileName)
{
    this->mem->loadRom(fileName);
}
