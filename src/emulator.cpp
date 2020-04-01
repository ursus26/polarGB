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

#include <fstream>
#include <chrono>
#include <fmt/format.h>
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
    this->isRunning = true;
    this->cyclesCompleted = 0;

    this->mmu = new Mmu();
    this->video = new Video();
    this->cpu = new Cpu();

    this->mmu->startUp(this->video);
    this->video->startUp(false);
    this->cpu->startUp(this->mmu);
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
    chrono::time_point<chrono::high_resolution_clock> start, end;
    chrono::duration<double> elapsed_time;

    /* Delta time takes into account that the elapsed_time > FRAME_TIME. If we reset the start time
     * of the frame we can lose a little bit of time. This variable calculates this difference
     * and helps make our timing function more accurate. */
    double delta_time = 0.0;
    start = chrono::high_resolution_clock::now();

    while(this->isRunning)
    {
        /* Measure the elapsed time. */
        end = chrono::high_resolution_clock::now();
        elapsed_time = end - start;

        /* Check if we need to run the gameboy for an entire frame. */
        if(elapsed_time.count() + delta_time >= FRAME_TIME)
        {
            /* Reset the timer and calculate the time we lost sleeping. */
            start = chrono::high_resolution_clock::now();
            delta_time = elapsed_time.count() + delta_time - FRAME_TIME;

            runFrame();
        }
    }
}


void Emulator::runFrame()
{
    u8 cpuCycles = 0;

    while(cyclesCompleted < INSTRUCTIONS_PER_FRAME)
    {
        /* CPU step. */
        cpuCycles = this->cpu->step();
        cyclesCompleted += cpuCycles;

        /* Update the screen with the same amount of cycles. */
        this->video->update(cpuCycles);
    }

    // /* Check if we should close our window. */
    // if(this->video->closeWindow())
    //     this->isRunning = false;

    cyclesCompleted -= INSTRUCTIONS_PER_FRAME;
}
