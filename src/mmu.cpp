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
#include <assert.h>
#include <fmt/format.h>
#include "polarGB/mmu.h"


using namespace std;


Mmu::Mmu()
{
    this->interruptController = nullptr;
    this->graphicsController = nullptr;
    this->timer = nullptr;

    this->ERAM.size = 0;
    this->ERAM.mem = nullptr;
    this->WRAM.size = 0;
    this->WRAM.mem = nullptr;
    this->HardwareRegisters.size = 0;
    this->HardwareRegisters.mem = nullptr;
    this->HRAM.size = 0;
    this->HRAM.mem = nullptr;
}

Mmu::~Mmu()
{
}


/**
 * Small boot program that initializes specific memory locations.
 */
void Mmu::startUp(GraphicsController* gc, InterruptController* interruptController, Timer* timer)
{
    assert(gc != nullptr);
    assert(interruptController != nullptr);
    assert(timer != nullptr);

    ERAM.size = ERAM_END_ADDR - ERAM_START_ADDR + 1;
    ERAM.mem = new u8[ERAM.size]();

    WRAM.size = WRAM_END_ADDR - WRAM_START_ADDR + 1;
    WRAM.mem = new u8[WRAM.size]();

    HardwareRegisters.size = HARDWARE_REGISTERS_END_ADDR - HARDWARE_REGISTERS_START_ADDR + 1;
    HardwareRegisters.mem = new u8[HardwareRegisters.size]();

    HRAM.size = HRAM_END_ADDR - HRAM_START_ADDR + 1;
    HRAM.mem = new u8[HRAM.size]();

    this->interruptController = interruptController;
    this->timer = timer;
    this->graphicsController = gc;
    this->rom.startUp();

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
    this->graphicsController = nullptr;
    this->interruptController = nullptr;
    this->timer = nullptr;

    delete[] ERAM.mem;
    ERAM.mem = nullptr;
    ERAM.size = 0;

    delete[] WRAM.mem;
    WRAM.mem = nullptr;
    WRAM.size = 0;

    delete[] HardwareRegisters.mem;
    HardwareRegisters.mem = nullptr;
    HardwareRegisters.size = 0;

    delete[] HRAM.mem;
    HRAM.mem = nullptr;
    HRAM.size = 0;
}


u8 Mmu::read(u16 addr)
{
    assert(this->graphicsController != nullptr);

    u8 data = 0;

    if(addr <= ROM_END_ADDR) /* ROM banks */
        data = rom.read(addr);
    else if(addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR) /* VRAM / LCD Display RAM */
        data = this->graphicsController->vramRead(addr - VRAM_START_ADDR);
    else if(addr >= ERAM_START_ADDR && addr <= ERAM_END_ADDR) /* Switchable external RAM bank */
        data = ERAM.mem[addr - ERAM_START_ADDR];
    else if(addr >= WRAM_START_ADDR && addr <= WRAM_END_ADDR) /* Working RAM bank 0 */
        data = WRAM.mem[addr - WRAM_START_ADDR];
    else if(addr > WRAM_END_ADDR && addr < OAM_START_ADDR) /* Echo ram, typically not used. */
        fmt::print(stderr, "Error, read request for echo RAM is not supported\n");
    else if(addr >= OAM_START_ADDR && addr <= OAM_END_ADDR) /* Sprite attribute table / OAM (Object Actribute Mem) */
        data = this->graphicsController->oamRead(addr - OAM_START_ADDR);
    else if(addr > OAM_END_ADDR && addr < HARDWARE_REGISTERS_START_ADDR) /* Not usable */
        fmt::print(stderr, "Error, read request for unusable memory at address: {:#x}\n", addr);
    else if(addr >= HARDWARE_REGISTERS_START_ADDR && addr <= HARDWARE_REGISTERS_END_ADDR) /* I/O Ports */
        data = readHardwareRegister(addr);
    else if(addr >= HRAM_START_ADDR && addr <= HRAM_END_ADDR) /* High RAM (HRAM) */
        data = HRAM.mem[addr - HRAM_START_ADDR];
    else if(addr == IE_ADDR)
        data = this->interruptController->getIE();

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
    assert(graphicsController != nullptr);

    if(addr <= ROM_END_ADDR) /* ROM banks */
        rom.write(addr, data);
    else if(addr >= VRAM_START_ADDR && addr <= VRAM_END_ADDR) /* VRAM / LCD Display RAM */
        this->graphicsController->vramWrite(addr - VRAM_START_ADDR, data);
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
        this->graphicsController->oamWrite(addr - OAM_START_ADDR, data);
    // else if(addr > OAM_END_ADDR && addr < HARDWARE_REGISTER_START_ADDR) /* Not usable */
        // fmt::print(stderr, "Error, write request for unusable memory at address: {:#x}, data: {:#x}\n", addr, data);
    else if(addr >= HARDWARE_REGISTERS_START_ADDR && addr <= HARDWARE_REGISTERS_END_ADDR) /* I/O Ports */
        writeHardwareRegister(addr, data);
    else if(addr >= HRAM_START_ADDR && addr <= HRAM_END_ADDR) /* High RAM (HRAM) */
        HRAM.mem[addr - HRAM_START_ADDR] = data;
    else if(addr == IE_ADDR)
        this->interruptController->setIE(data);
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


void Mmu::DMATransfer(u8 index)
{
    u16 sourceAddress = ((u16)index) << 8;
    u16 destinationAddress = OAM_START_ADDR;
    for(u8 i = 0; i < 0xa0; i++)
    {
        u8 data = this->read(sourceAddress + i);
        this->write(destinationAddress + i, data);
    }
}

u8 Mmu::readHardwareRegister(u16 addr)
{
    assert(graphicsController != nullptr);
    assert(interruptController != nullptr);
    assert(addr >= HARDWARE_REGISTERS_START_ADDR);
    assert(addr <= HARDWARE_REGISTERS_END_ADDR);

    switch(addr)
    {
        case DIV_ADDR:  return timer->read(RegDIV);
        case TIMA_ADDR: return timer->read(RegTIMA);
        case TMA_ADDR:  return timer->read(RegTMA);
        case TAC_ADDR:  return timer->read(RegTAC);
        case LCDC_ADDR: return graphicsController->displayRegisterRead(RegLCDC);
        case STAT_ADDR: return graphicsController->displayRegisterRead(RegSTAT);
        case SCY_ADDR:  return graphicsController->displayRegisterRead(RegSCY);
        case SCX_ADDR:  return graphicsController->displayRegisterRead(RegSCX);
        case LY_ADDR:   return graphicsController->displayRegisterRead(RegLY);
        case LYC_ADDR:  return graphicsController->displayRegisterRead(RegLYC);
        case DMA_ADDR:  return graphicsController->displayRegisterRead(RegDMA);
        case BGP_ADDR:  return graphicsController->displayRegisterRead(RegBGP);
        case OBP0_ADDR: return graphicsController->displayRegisterRead(RegOBP0);
        case OBP1_ADDR: return graphicsController->displayRegisterRead(RegOBP1);
        case WY_ADDR:   return graphicsController->displayRegisterRead(RegWY);
        case WX_ADDR:   return graphicsController->displayRegisterRead(RegWX);
        case IF_ADDR:   return interruptController->getIF();
        case IE_ADDR:   return interruptController->getIE();
        default:        return HardwareRegisters.mem[addr - HARDWARE_REGISTERS_START_ADDR];
    }
}


void Mmu::writeHardwareRegister(u16 addr, u8 data)
{
    assert(graphicsController != nullptr);
    assert(interruptController != nullptr);
    assert(addr >= HARDWARE_REGISTERS_START_ADDR);
    assert(addr <= HARDWARE_REGISTERS_END_ADDR);

    switch(addr)
    {
        case DIV_ADDR:  timer->write(RegDIV, data); break;
        case TIMA_ADDR: timer->write(RegTIMA, data); break;
        case TMA_ADDR:  timer->write(RegTMA, data); break;
        case TAC_ADDR:  timer->write(RegTAC, data); break;
        case LCDC_ADDR: graphicsController->displayRegisterWrite(RegLCDC, data); break;
        case STAT_ADDR: graphicsController->displayRegisterWrite(RegSTAT, data); break;
        case SCY_ADDR:  graphicsController->displayRegisterWrite(RegSCY, data); break;
        case SCX_ADDR:  graphicsController->displayRegisterWrite(RegSCX, data); break;
        case LY_ADDR:   graphicsController->displayRegisterWrite(RegLY, data); break;
        case LYC_ADDR:  graphicsController->displayRegisterWrite(RegLYC, data); break;
        case DMA_ADDR:
            DMATransfer(data);
            graphicsController->displayRegisterWrite(RegDMA, data);
            break;
        case BGP_ADDR:  graphicsController->displayRegisterWrite(RegBGP, data); break;
        case OBP0_ADDR: graphicsController->displayRegisterWrite(RegOBP0, data); break;
        case OBP1_ADDR: graphicsController->displayRegisterWrite(RegOBP1, data); break;
        case WY_ADDR:   graphicsController->displayRegisterWrite(RegWY, data); break;
        case WX_ADDR:   graphicsController->displayRegisterWrite(RegWX, data); break;
        case IF_ADDR:   interruptController->setIF(data); break;
        case IE_ADDR:   interruptController->setIE(data); break;
        default:        HardwareRegisters.mem[addr - HARDWARE_REGISTERS_START_ADDR] = data; break;
    }
}
