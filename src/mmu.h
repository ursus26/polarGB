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
    void boot();

    /* Read from an address. */
    u8 read(u16 addr);
    u16 read16bits(u16 addr);

    /* Write data to memory. */
    void write(u16 addr, u8 data);

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
