#include <iostream>
#include <stdio.h>
#include "memory_manager.h"

using namespace std;

MemoryManager::MemoryManager()
{
    cout << "Initialize memory manager" << endl;

    VRAM.size = VRAM_END_ADDR - VRAM_START_ADDR + 1;
    VRAM.mem = new U8[VRAM.size];

    ERAM.size = ERAM_END_ADDR - ERAM_START_ADDR + 1;
    ERAM.mem = new U8[ERAM.size];

    WRAM.size = WRAM_END_ADDR - WRAM_START_ADDR + 1;
    WRAM.mem = new U8[WRAM.size];

    HRAM.size = HRAM_END_ADDR - HRAM_START_ADDR + 1;
    HRAM.mem = new U8[HRAM.size];
}

MemoryManager::~MemoryManager()
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

U8 MemoryManager::fetch(U16 addr)
{
    // cout << "It should load some memory at address: " << hex << addr << dec << endl;

    U8 data = 0;

    if(addr <= 0x7FFF) /* ROM banks */
        data = rom.load(addr);
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
U16 MemoryManager::fetchU16(U16 addr)
{
    /* Get the low and high U8 of the 16 bit address. */
    U16 low = fetch(addr);
    U16 high = fetch(addr + 1);

    /* Construct the address. */
    U16 word = (high << 8) + low;

    return word;
}


void MemoryManager::loadRom(string fileName)
{
    this->rom.loadCartridge(fileName);
}


void MemoryManager::write(U16 addr, U8 data)
{
    cout << "WRITE ACTION: 0x" << hex << addr << " <- 0x";
    printf("%01i", data);
    cout << dec << endl;
}
