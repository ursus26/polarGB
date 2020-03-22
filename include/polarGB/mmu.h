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

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include "types.h"
#include "cartridge.h"


typedef struct
{
    u8* mem;
    unsigned int size;
} Ram;


const u16 ROM_END_ADDR      = 0x7fff;
const u16 VRAM_START_ADDR   = 0x8000;
const u16 VRAM_END_ADDR     = 0x9fff;
const u16 ERAM_START_ADDR   = 0xa000;
const u16 ERAM_END_ADDR     = 0xbfff;
const u16 WRAM_START_ADDR   = 0xc000;
const u16 WRAM_END_ADDR     = 0xdfff;
const u16 HRAM_START_ADDR   = 0xfe00;
const u16 HRAM_END_ADDR     = 0xffff;


class Mmu
{
public:
    Mmu();
    ~Mmu();

    /* Small boot program for the mmu. */
    void startUp();
    void shutDown();

    /* Read from an address. */
    u8 read(u16 addr);
    u16 read2Bytes(u16 addr);

    /* Write data to memory. */
    void write(u16 addr, u8 data);
    void write2Bytes(u16 addr, u16 data);

    /* Load a rom file into memory. */
    void loadRom(std::string fileName);

private:
    Cartridge rom;  /* Game cartridge */
    Ram VRAM;       /* Video RAM */
    Ram ERAM;       /* External RAM */
    Ram WRAM;       /* Working RAM */
    Ram HRAM;       /* High Ram / CPU working RAM */

    void DMATransfer(u8 index);
};

#endif /* MEMORY_MANAGER_H */
