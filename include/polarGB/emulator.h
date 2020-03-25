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

#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include "types.h"
#include "mmu.h"
#include "cpu.h"
#include "video.h"


const int CPU_FREQUENCY = 1048576; /* Hz */
const int INSTRUCTIONS_PER_FRAME = 17556;
const double FPS = CPU_FREQUENCY / (double)INSTRUCTIONS_PER_FRAME;
const double FRAME_TIME = 1.0 / FPS;


class Emulator
{
public:
    Emulator();
    ~Emulator();

    int start(std::string cartridgePath);

private:
    bool isRunning;
    u64 cyclesCompleted;
    Mmu* mmu;
    Cpu* cpu;
    Video* video;

    void startUp();
    void shutDown();
    void run();
    void runFrame();
};

#endif /* EMULATOR_H */
