#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include "types.h"
#include "cartridge.h"
#include "bank_manager.h"


typedef struct
{
    U8* mem;
    unsigned int size;
} Ram;


const U16 ROM_END_ADDR = 0x7FFF;
const U16 VRAM_START_ADDR = 0x8000;
const U16 VRAM_END_ADDR = 0x9FFF;
const U16 ERAM_START_ADDR = 0xA000;
const U16 ERAM_END_ADDR = 0xBFFF;
const U16 WRAM_START_ADDR = 0xC000;
const U16 WRAM_END_ADDR = 0xDFFF;
const U16 HRAM_START_ADDR = 0xFE00;
const U16 HRAM_END_ADDR = 0xFFFF;


class MemoryManager
{
public:
    MemoryManager();
    ~MemoryManager();

    U8 fetch(U16 addr);
    U16 fetchU16(U16 addr);
    void loadRom(std::string fileName);

    void write(U16 addr, U8 data);

private:
    Cartridge rom;  /* Game cartridge */
    Ram VRAM;       /* Video RAM */
    Ram ERAM;       /* External RAM */
    Ram WRAM;       /* Working RAM */
    Ram HRAM;       /* High Ram / CPU working RAM */
};

#endif /* MEMORY_MANAGER_H */
