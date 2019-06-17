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
