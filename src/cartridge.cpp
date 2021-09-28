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
#include <stdexcept>
#include <fstream>
#include <fmt/format.h>
#include "polarGB/cartridge.h"


using namespace std;


Cartridge::Cartridge()
{
    this->mem = nullptr;
    this->size = 0;
    this->fileName = "";
    this->gameTitle = "";
    this->CGBFlag = 0;
    this->SGBFlag = false;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->ramSize = 0;
    this->destinationCode = 0;

}

Cartridge::~Cartridge()
{
    this->removeLoadedCartridge();
}


void Cartridge::load(string fileName)
{
    if(this->mem != nullptr || this->size > 0)
    {
        fmt::print("Warning, a different file is already loaded in. New file '{}' will replace the old file '{}'\n", fileName, this->fileName);
        removeLoadedCartridge();
    }

    this->loadFile(fileName);
}


void Cartridge::removeLoadedCartridge()
{
    delete[] this->mem;
    this->mem = nullptr;
    this->size = 0;
    this->fileName = "";
    this->gameTitle = "";
    this->CGBFlag = 0;
    this->SGBFlag = false;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->ramSize = 0;
    this->destinationCode = 0;
}


void Cartridge::loadFile(string fileName)
{
    fmt::print("Loading a new cartridge from file: {}\n", fileName);
    this->fileName = fileName;

    ifstream f;
    f.open(fileName, ios::in | ios::binary);
    if(!f.is_open())
    {
        throw std::runtime_error(
            fmt::format("Could not open file: '{}'", fileName)
        );
    }

    this->size = getFileSize(&f);
    this->mem = new uint8_t[this->size];
    for(unsigned int i = 0; i < this->size && f.good(); i++)
        this->mem[i] = (uint8_t) f.get();
    f.close();

    processCartridgeHeader();
}


u8 Cartridge::read(u16 address)
{
    if(address > this->size || this->mem == nullptr)
        return 0;

    return mem[address];
}


void Cartridge::write(u16 address, u8 data)
{
    if(address > this->size || this->mem == nullptr)
    {
        fmt::print(stderr, "{}: Error, invalid write request\n");
        return;
    }

    mem[address] = data;
}


unsigned int Cartridge::getFileSize(ifstream *f)
{
    unsigned int fileSize = 0;
    if(f->is_open() && f->good())
    {
        f->seekg (0, f->end);
        fileSize = f->tellg();
        f->seekg (0, f->beg);
    }
    return fileSize;
}


/**
 * More info can be found here: http://gbdev.gg8.se/wiki/articles/The_Cartridge_Header
 */
void Cartridge::processCartridgeHeader()
{
    /* Read the game title. */
    this->gameTitle = "";
    for(int i = 0x134; i < 0x144; i++)
    {
        u8 character = this->read(i);
        if(character == 0)
            break;

        gameTitle += character;
    }

    /* Read the SGB Flag. */
    if(this->read(0x146) == 0x03)
        SGBFlag = true;
    else
        SGBFlag = false;

    cartridgeType = this->read(0x147);
    romSize = this->read(0x148);
    ramSize = this->read(0x149);
    destinationCode = this->read(0x14a);
    this->checksum();
}


void Cartridge::printInfo()
{
    fmt::print("Game Title: {}\nCartridge Type: {:#x}\n", this->gameTitle, this->cartridgeType);
}


bool Cartridge::checksum()
{
    u8 checksum = 0x19;
    for(u16 addr = 0x0134; addr <= 0x014d; addr++)
        checksum += this->read(addr);

    if(checksum != 0)
    {
        throw std::runtime_error(
            fmt::format("Cartridge header checksum failed. File '{}' is probably not a valid Game Boy ROM.", this->fileName)
        );
    }

    fmt::print("Checksum: PASSED\n");
    return true;
}
