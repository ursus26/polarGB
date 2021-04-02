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
#include "interrupt_controller.h"
#include "graphics_controller.h"


/* Memory boundaries. */
const u16 ROM_END_ADDR                  = 0x7fff;
const u16 VRAM_START_ADDR               = 0x8000;
const u16 VRAM_END_ADDR                 = 0x9fff;
const u16 ERAM_START_ADDR               = 0xa000;
const u16 ERAM_END_ADDR                 = 0xbfff;
const u16 WRAM_START_ADDR               = 0xc000;
const u16 WRAM_END_ADDR                 = 0xdfff;
const u16 OAM_START_ADDR                = 0xfe00;
const u16 OAM_END_ADDR                  = 0xfe9f;
const u16 HARDWARE_REGISTERS_START_ADDR  = 0xff00;
const u16 HARDWARE_REGISTERS_END_ADDR    = 0xff7f;
const u16 HRAM_START_ADDR               = 0xff80;
const u16 HRAM_END_ADDR                 = 0xfffe;

/* LCD display registers addresses. */
const u16 LCDC_ADDR     = 0xff40;
const u16 STAT_ADDR     = 0xff41;
const u16 SCY_ADDR      = 0xff42;
const u16 SCX_ADDR      = 0xff43;
const u16 LY_ADDR       = 0xff44;
const u16 LYC_ADDR      = 0xff45;
const u16 DMA_ADDR      = 0xff46;
const u16 BGP_ADDR      = 0xff47;
const u16 OBP0_ADDR     = 0xff48;
const u16 OBP1_ADDR     = 0xff49;
const u16 WY_ADDR       = 0xff4a;
const u16 WX_ADDR       = 0xff4b;

/* Interrupt flag registers addresses. */
const u16 IF_ADDR = 0xff0f;
const u16 IE_ADDR = 0xffff;

class Mmu
{
public:
    Mmu();
    ~Mmu();

    /* Small boot program for the mmu. */
    void startUp(GraphicsController* gc, InterruptController* interruptController);
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
    ram_t ERAM;       /* External RAM */
    ram_t WRAM;       /* Working RAM */
    ram_t OAM;
    ram_t HardwareRegisters;
    ram_t HRAM;       /* High Ram / CPU working RAM */

    InterruptController* interruptController;
    GraphicsController* graphicsController;

    void DMATransfer(u8 index);
    u8 readHardwareRegister(u16 addr);
    void writeHardwareRegister(u16 addr, u8 data);
};

#endif /* MEMORY_MANAGER_H */
