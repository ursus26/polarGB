#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "mmu.h"


using namespace std;


Mmu::Mmu()
{
    cout << "Initialize Mmu" << endl;

    VRAM.size = VRAM_END_ADDR - VRAM_START_ADDR + 1;
    VRAM.mem = new U8[VRAM.size]();

    ERAM.size = ERAM_END_ADDR - ERAM_START_ADDR + 1;
    ERAM.mem = new U8[ERAM.size]();

    WRAM.size = WRAM_END_ADDR - WRAM_START_ADDR + 1;
    WRAM.mem = new U8[WRAM.size]();

    HRAM.size = HRAM_END_ADDR - HRAM_START_ADDR + 1;
    HRAM.mem = new U8[HRAM.size]();
}

Mmu::~Mmu()
{
    delete[] VRAM.mem;
    VRAM.mem = nullptr;

    delete[] ERAM.mem;
    ERAM.mem = nullptr;

    delete[] WRAM.mem;
    WRAM.mem = nullptr;

    delete[] HRAM.mem;
    HRAM.mem = nullptr;
}

/**
 * Small boot program that initializes specific memory locations.
 */
void Mmu::boot()
{
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
    this->write(0xff40, 0x91);   /* LCDC */
    this->write(0xff42, 0x00);   /* SCY */
    this->write(0xff43, 0x00);   /* SCX */
    this->write(0xff45, 0x00);   /* LYC */
    this->write(0xff47, 0xfc);   /* BGP */
    this->write(0xff48, 0xff);   /* OBP0 */
    this->write(0xff49, 0xff);   /* OBP1 */
    this->write(0xff4a, 0x00);   /* WY */
    this->write(0xff4b, 0x00);   /* WX */
    HRAM.mem[0xff] = 0x00;       /* IE, also the only time we allow writing to this location. */
}


U8 Mmu::read(U16 addr)
{
    U8 data = 0;

    if(addr <= 0x7FFF) /* ROM banks */
        data = rom.read(addr);
    else if(addr >= 0x8000 && addr <= 0x9FFF) /* VRAM */
        data = VRAM.mem[addr - 0x8000];
    else if(addr >= 0xA000 && addr <= 0xBFFF) /* Switchable external RAM bank */
        data = ERAM.mem[addr - 0xA000];
    else if(addr >= 0xC000 && addr <= 0xCFFF) /* Working RAM bank 0 */
        data = WRAM.mem[addr - 0xC000];
    else if(addr >= 0xD000 && addr <= 0xDFFF) /* Working RAM bank 1 */
        data = WRAM.mem[addr - 0xC000];
    else if(addr >= 0xE000 && addr <= 0xFDFF) /* Echo ram, typically not used. */
        cerr << "Error, unsuported echo RAM" << endl;
    else if(addr >= 0xFE00 && addr <= 0xFE9F) /* Sprite attribute table */
        data = HRAM.mem[addr - 0xFF00];
    else if(addr >= 0xFEA0 && addr <= 0xFEFF) /* Not usable */
        cerr << "Error, request for Unusable memory" << endl;
    else if(addr >= 0xFF00 && addr <= 0xFF7F) /* I/O Ports */
        data = HRAM.mem[addr - 0xFF00];
    else if(addr >= 0xFF80 && addr <= 0xFFFE) /* High RAM (HRAM) */
        data = HRAM.mem[addr - 0xFF00];
    else if(addr == 0xFFFF) /* Interrupt enable register */
        data = 0;

    return data;
}

/**
 * Fetches 16 bits from memory by stitching 2 bytes together. The first byte is the low byte and
 * the second byte is the high byte.
 */
U16 Mmu::readU16(U16 addr)
{
    /* Get the low and high U8 of the 16 bit address. */
    U16 low = read(addr);
    U16 high = read(addr + 1);

    /* Construct the address. */
    U16 word = (high << 8) + low;

    return word;
}


void Mmu::loadRom(string fileName)
{
    this->rom.loadCartridge(fileName);
    this->rom.printInfo();
}


void Mmu::write(U16 addr, U8 data)
{
    cout << "WRITE ACTION: 0x" << hex << addr << " <- ";
    printf("0x%02x", data);
    cout << dec << endl;


    if(addr <= 0x7FFF) /* ROM banks */
    {
        cerr << "Error, unsuported write action for cartridge roms" << endl;
        exit(EXIT_FAILURE);
    }
    else if(addr >= 0x8000 && addr <= 0x9FFF) /* VRAM */
        VRAM.mem[addr - 0x8000] = data;
    else if(addr >= 0xA000 && addr <= 0xBFFF) /* Switchable external RAM bank */
        ERAM.mem[addr - 0xA000] = data;
    else if(addr >= 0xC000 && addr <= 0xCFFF) /* Working RAM bank 0 */
        WRAM.mem[addr - 0xC000] = data;
    else if(addr >= 0xD000 && addr <= 0xDFFF) /* Working RAM bank 1 */
        WRAM.mem[addr - 0xC000] = data;
    else if(addr >= 0xE000 && addr <= 0xFDFF) /* Echo ram, typically not used. */
    {
        cerr << "Error, unsuported echo RAM" << endl;
        exit(EXIT_FAILURE);
    }
    else if(addr >= 0xFE00 && addr <= 0xFE9F) /* Sprite attribute table */
        HRAM.mem[addr - 0xFF00] = data;
    else if(addr >= 0xFEA0 && addr <= 0xFEFF) /* Not usable */
        cerr << "Error, write request for unusable memory" << endl;
    else if(addr >= 0xFF00 && addr <= 0xFF7F) /* I/O Ports */
        HRAM.mem[addr - 0xFF00] = data;
    else if(addr >= 0xFF80 && addr <= 0xFFFE) /* High RAM (HRAM) */
        HRAM.mem[addr - 0xFF00] = data;
    else if(addr == 0xFFFF) /* Interrupt enable register */
    {
        cerr << "Error, unsuported write action for interrupt enable register" << endl;
        exit(EXIT_FAILURE);
    }
}
