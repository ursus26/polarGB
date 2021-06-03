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

#include <memory>
#include <string>
#include "types.h"
#include "cartridge.h"
#include "graphics_controller.h"
#include "interrupt_controller.h"
#include "joypad.h"
#include "timer.h"


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

/* Joypad register address */
const u16 P1_ADDR = 0xff00;

/* Serial data transfer register addresses */
const u16 SB_ADDR = 0xff01;
const u16 SC_ADDR = 0xff02;

/* Timer registers addresses */
const u16 DIV_ADDR = 0xff04;
const u16 TIMA_ADDR = 0xff05;
const u16 TMA_ADDR = 0xff06;
const u16 TAC_ADDR = 0xff07;

/* Sound controller register addressers */
const u16 NR10_ADDR = 0xff10;
const u16 NR11_ADDR = 0xff11;
const u16 NR12_ADDR = 0xff12;
const u16 NR13_ADDR = 0xff13;
const u16 NR14_ADDR = 0xff14;
const u16 NR21_ADDR = 0xff16;
const u16 NR22_ADDR = 0xff17;
const u16 NR23_ADDR = 0xff18;
const u16 NR24_ADDR = 0xff19;
const u16 NR30_ADDR = 0xff1a;
const u16 NR31_ADDR = 0xff1b;
const u16 NR32_ADDR = 0xff1c;
const u16 NR33_ADDR = 0xff1d;
const u16 NR34_ADDR = 0xff1e;
const u16 FF3F_ADDR = 0xff30;
const u16 NR41_ADDR = 0xff20;
const u16 NR42_ADDR = 0xff21;
const u16 NR43_ADDR = 0xff22;
const u16 NR44_ADDR = 0xff23;
const u16 NR50_ADDR = 0xff24;
const u16 NR51_ADDR = 0xff25;
const u16 NR52_ADDR = 0xff26;

/* LCD display registers addresses */
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

/* Interrupt flag registers addresses */
const u16 IF_ADDR = 0xff0f;
const u16 IE_ADDR = 0xffff;


class Mmu
{
public:
    Mmu(std::shared_ptr<GraphicsController> gc, std::shared_ptr<InterruptController> ic,
        std::shared_ptr<Timer> timer, std::shared_ptr<Joypad> joypad);
    ~Mmu();

    /* Small boot program for the mmu. */
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
    ram_t HardwareRegisters;
    ram_t HRAM;       /* High Ram / CPU working RAM */

    std::shared_ptr<GraphicsController> graphicsController;
    std::shared_ptr<InterruptController> interruptController;
    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;

    void initializeMemory();
    void DMATransfer(u8 index);
    u8 readHardwareRegister(u16 addr);
    void writeHardwareRegister(u16 addr, u8 data);
};

#endif /* MEMORY_MANAGER_H */
