/**
 * Copyright (C) 2018 Bart de Haan
 *
 * polarGB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * polarGB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <stdio.h>
#include <fstream>
#include "polarGB/emulator.h"

using namespace std;

Emulator::Emulator()
{
    this->mmu = new Mmu();
    this->video = new Video();
    this->video->initialise();
    this->cpu = new Cpu(this->mmu, this->video);
}

Emulator::~Emulator()
{
    delete this->cpu;
    this->cpu = nullptr;

    delete this->video;
    this->video = nullptr;

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
