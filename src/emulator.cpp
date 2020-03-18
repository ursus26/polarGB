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
#include <chrono>
#include "polarGB/emulator.h"


using namespace std;


Emulator::Emulator()
{
}

Emulator::~Emulator()
{
}


int Emulator::start(string cartridgePath)
{
    /* Initialise all the subsystems of the gameboy emulator. */
    this->startUp();

    /* Load the game cartridge. */
    this->mmu->loadRom(cartridgePath);

    /* Enter the emulator loop. */
    this->run();

    /* Shut down the gameboy emulator. */
    this->shutDown();
    return 0;
}


void Emulator::startUp()
{
    this->mmu = new Mmu();
    this->mmu->startUp();

    this->video = new Video();
    this->video->startUp();

    this->cpu = new Cpu();
    this->cpu->startUp(this->mmu, this->video);
}


void Emulator::shutDown()
{
    this->cpu->shutDown();
    delete this->cpu;
    this->cpu = nullptr;

    this->video->shutDown();
    delete this->video;
    this->video = nullptr;

    this->mmu->shutDown();
    delete this->mmu;
    this->mmu = nullptr;
}


void Emulator::run()
{
    // cpu->runNCycles(15);
    // cpu->runSingleFrame();
    // cpu->runSingleFrame();

    /* Run the emulator. */
    cpu->run();
}
