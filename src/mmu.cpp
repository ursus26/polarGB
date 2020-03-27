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

#include <stdlib.h>
#include <cstring>
#include <fmt/format.h>
#include "polarGB/mmu.h"


using namespace std;


Mmu::Mmu()
{
}

Mmu::~Mmu()
{
}


/**
 * Small boot program that initializes specific memory locations.
 */
void Mmu::startUp()
{
    /* Allocate the memory areas. */
    VRAM.size = VRAM_END_ADDR - VRAM_START_ADDR + 1;
    VRAM.mem = new u8[VRAM.size]();

    ERAM.size = ERAM_END_ADDR - ERAM_START_ADDR + 1;
    ERAM.mem = new u8[ERAM.size]();

    WRAM.size = WRAM_END_ADDR - WRAM_START_ADDR + 1;
    WRAM.mem = new u8[WRAM.size]();

    OAM.size = OAM_END_ADDR - OAM_START_ADDR + 1;
    OAM.mem = new u8[OAM.size]();

    HardwareRegisters.size = HARDWARE_REGISTERS_END_ADDR - HARDWARE_REGISTERS_START_ADDR + 1;
    HardwareRegisters.mem = new u8[HardwareRegisters.size]();

    HRAM.size = HRAM_END_ADDR - HRAM_START_ADDR + 1;
    HRAM.mem = new u8[HRAM.size]();

    this->rom.startUp();

    /* First clear vram. */
    memset(VRAM.mem, 0, sizeof(VRAM.size));

    /* Reference: http://gbdev.gg8.se/wiki/articles/Power_Up_Sequence */
    this->write(0xff05, 0x00);   /* TIMA */
    this->write(0xff06, 0x00);   /* TMA */
    this->write(0xff07, 0x00);   /* TAC */
    this->write(0xff10, 0x80);   /* NR10 */
    this->write(0xff11, 0xbf);   /* NR11 */
    this->write(0xff12, 0xf3);   /* NR12 */
    this->write(0xff14, 0xbf);   /* NR14 */
    this->write(0xff16, 0x3f);   /* NR21 */
    this->write(0xff17, 0x00);   /* NR22 */
    this->write(0xff19, 0xbf);   /* NR24 */
    this->write(0xff1a, 0x7f);   /* NR30 */
    this->write(0xff1b, 0xff);   /* NR31 */
    this->write(0xff1c, 0x9f);   /* NR32 */
    this->write(0xff1e, 0xbf);   /* NR33 */
    this->write(0xff20, 0xff);   /* NR41 */
    this->write(0xff21, 0x00);   /* NR42 */
    this->write(0xff22, 0x00);   /* NR43 */
    this->write(0xff23, 0xbf);   /* NR44 */
    this->write(0xff24, 0x77);   /* NR50 */
    this->write(0xff25, 0xf3);   /* NR51 */
    this->write(0xff26, 0xf1);   /* NR52 */
    this->write(LCDC_ADDR, 0x91);
    this->write(SCY_ADDR, 0x00);
    this->write(SCX_ADDR, 0x00);
    this->write(LYC_ADDR, 0x00);
    this->write(BGP_ADDR, 0xfc);
    this->write(OBP0_ADDR, 0xff);
    this->write(OBP1_ADDR, 0xff);
    this->write(WY_ADDR, 0x00);
    this->write(WX_ADDR, 0x00);
    this->write(IE_ADDR, 0x00);
}


/**
 * Clean up all the memory allocated for the gameboy.
 */
void Mmu::shutDown()
{
    this->rom.shutDown();

    delete[] VRAM.mem;
    VRAM.mem = nullptr;

    delete[] ERAM.mem;
    ERAM.mem = nullptr;

    delete[] WRAM.mem;
    WRAM.mem = nullptr;

    delete[] OAM.mem;
    OAM.mem = nullptr;

    delete[] HardwareRegisters.mem;
    HardwareRegisters.mem = nullptr;

    delete[] HRAM.mem;
    HRAM.mem = nullptr;
}


u8 Mmu::read(u16 addr)
{
    u8 data = 0;

    if(addr <= ROM_END_ADDR) /* ROM banks */
        data = rom.read(addr);
    else if(addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR) /* VRAM / LCD Display RAM */
        data = VRAM.mem[addr - VRAM_START_ADDR];
    else if(addr >= ERAM_START_ADDR && addr <= ERAM_END_ADDR) /* Switchable external RAM bank */
        data = ERAM.mem[addr - ERAM_START_ADDR];
    else if(addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR) /* Working RAM bank 0 */
        data = WRAM.mem[addr - WRAM_START_ADDR];
    else if(addr > WRAM_END_ADDR && addr < OAM_START_ADDR) /* Echo ram, typically not used. */
        fmt::print(stderr, "Error, read request for echo RAM is not supported\n");
    else if(addr >= OAM_START_ADDR && addr <= OAM_END_ADDR) /* Sprite attribute table / OAM (Object Actribute Mem) */
        data = OAM.mem[addr - OAM_START_ADDR];
    else if(addr > OAM_END_ADDR && addr < HARDWARE_REGISTERS_START_ADDR) /* Not usable */
        fmt::print(stderr, "Error, read request for unusable memory at address: {:#x}\n", addr);
    else if(addr >= HARDWARE_REGISTERS_START_ADDR && addr <= HARDWARE_REGISTERS_END_ADDR) /* I/O Ports */
        data = HardwareRegisters.mem[addr - HARDWARE_REGISTERS_START_ADDR];
    else if(addr >= HRAM_START_ADDR) /* High RAM (HRAM) */
        data = HRAM.mem[addr - HRAM_START_ADDR];

    return data;
}

/**
 * Fetches 16 bits from memory by stitching 2 bytes together. The first byte is the low byte and
 * the second byte is the high byte.
 */
u16 Mmu::read2Bytes(u16 addr)
{
    /* Get the low and high u8 of the 16 bit address. */
    u16 low = read(addr);
    u16 high = read(addr + 1);

    /* Construct the address. */
    u16 word = (high << 8) + low;

    return word;
}


void Mmu::write(u16 addr, u8 data)
{
    if(addr <= ROM_END_ADDR) /* ROM banks */
        rom.write(addr, data);
    else if(addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR) /* VRAM / LCD Display RAM */
        VRAM.mem[addr - VRAM_START_ADDR] = data;
    else if(addr >= ERAM_START_ADDR && addr <= ERAM_END_ADDR) /* Switchable external RAM bank */
        ERAM.mem[addr - ERAM_START_ADDR] = data;
    else if(addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR) /* Working RAM bank 0 */
        WRAM.mem[addr - WRAM_START_ADDR] = data;
    else if(addr > WRAM_END_ADDR && addr < OAM_START_ADDR) /* Echo ram, typically not used. */
    {
        fmt::print(stderr, "Error, unsupported write action for echo RAM on address: {:#x}\n", addr);
        exit(EXIT_FAILURE);
    }
    else if(addr >= OAM_START_ADDR && addr <= OAM_END_ADDR) /* Sprite attribute table / OAM (Object Actribute Mem) */
        OAM.mem[addr - OAM_START_ADDR] = data;
    // else if(addr > OAM_END_ADDR && addr < HARDWARE_REGISTER_START_ADDR) /* Not usable */
        // fmt::print(stderr, "Error, write request for unusable memory at address: {:#x}, data: {:#x}\n", addr, data);
    else if(addr >= HARDWARE_REGISTERS_START_ADDR && addr <= HARDWARE_REGISTERS_END_ADDR) /* I/O Ports */
        HardwareRegisters.mem[addr - HARDWARE_REGISTERS_START_ADDR] = data;
    else if(addr >= HRAM_START_ADDR) /* High RAM (HRAM) */
        HRAM.mem[addr - HRAM_START_ADDR] = data;
}


/**
 * Writes two bytes of data to memory.
 */
void Mmu::write2Bytes(u16 addr, u16 data)
{
    u8 low = data & 0xff;
    u8 high = (data >> 8) & 0xff;

    this->write(addr, low);
    this->write(addr + 1, high);
}


void Mmu::loadRom(string fileName)
{
    this->rom.loadCartridge(fileName);
    this->rom.printInfo();
}


// void Mmu::DMATransfer(u8 index)
// {
//     u16 source_address = index << 8;
//     u16 dest_address = 0xfe00;
//     u8 data = 0x0;
//
//     for(u8 i = 0; i < 0xa0; i++)
//     {
//         data = this->read(source_address + i);
//         this->write(dest_address + i, data);
//     }
// }
