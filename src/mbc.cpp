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

#include "polarGB/mbc.h"
#include <fmt/format.h>


using namespace std;


MBC::MBC(int romSize, int ramSize)
{
    this->romSize = romSize;
    this->ramSize = ramSize;

    this->romMem.resize(romSize);
    this->ramMem.resize(ramSize);
}


MBC::~MBC()
{
    clear();
}


void MBC::loadROM(ifstream *f)
{
    f->seekg(0, f->beg);
    u8* data = this->romMem.data();
    f->read((char *)data, this->romSize);
}


void MBC::clear()
{
    this->romMem.clear();
    this->romSize = 0;

    this->ramMem.clear();
    this->ramSize = 0;
}


u8 MBC::read(u16 address)
{
    if(address < 0x8000)
    {
        return this->romMem[address];
    }
    else if(address >= 0xa000 && address <= 0xbfff)
    {
        return this->ramMem[address];
    }
    else
    {
        fmt::format("Invalid cartridge read operation at address {:#x}", address);
        return 0xff;
    }
}


void MBC::write(u16 address, u8 data)
{
    if(address < 0x8000)
    {
        this->romMem[address] = data;
    }
    else if(address >= 0xa000 && address <= 0xbfff && this->ramSize >= address - 0xa000)
    {
        this->ramMem[address - 0xa000] = data;
    }
    else
    {
        fmt::format("Invalid cartridge write operation at address {:#x} with data {:#x}", address, data);
    }
}


NoMBC::NoMBC(int romSize, int ramSize) : MBC(romSize, ramSize)
{
}

NoMBC::~NoMBC()
{
    clear();
}
