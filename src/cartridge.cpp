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

#include <stdexcept>
#include <fmt/format.h>
#include "polarGB/cartridge.h"


using namespace std;


Cartridge::Cartridge()
{
    this->mbc = nullptr;
    this->fileSize = 0;
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
    this->clear();
}


void Cartridge::load(string fileName)
{
    if(this->mbc != nullptr || this->fileSize > 0)
    {
        fmt::print("Warning, a different file is already loaded in. New file '{}' will replace the old file '{}'\n", fileName, this->fileName);
        this->clear();
    }

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

    this->fileSize = getFileSize(&f);
    processCartridgeHeader(&f);
    initializeMBC(&f);
    f.close();
}


/**
 * Clears the cartridge from memory.
 */
void Cartridge::clear()
{
    if(this->mbc != nullptr)
    {
        delete this->mbc;
        this->mbc = nullptr;
    }

    this->fileSize = 0;
    this->fileName = "";
    this->gameTitle = "";
    this->CGBFlag = 0;
    this->SGBFlag = false;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->ramSize = 0;
    this->destinationCode = 0;
}


u8 Cartridge::read(u16 address)
{
    return this->mbc->read(address);
    // if(address > this->size || this->mem == nullptr)
    //     return 0;
    //
    // return mem[address];
}


void Cartridge::write(u16 address, u8 data)
{
    // if(address > this->size || this->mem == nullptr)
    // {
    //     fmt::print(stderr, "{}: Error, invalid write request\n");
    //     return;
    // }
    //
    // mem[address] = data;

    return this->mbc->write(address, data);
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
void Cartridge::processCartridgeHeader(ifstream *f)
{
    u8* buffer = new u8[0x150];
    f->seekg(0, f->beg);
    f->read((char *)buffer, 0x150);

    /* Read the game title. */
    this->gameTitle = "";
    this->gameTitle = string((char *)(buffer + 0x134), 0x144 - 0x134);

    SGBFlag = (buffer[0x146] == 0x03) ? true : false;
    cartridgeType = buffer[0x147];
    romSize = buffer[0x148];
    ramSize = buffer[0x149];
    destinationCode = buffer[0x14a];
    this->checksum(buffer);

    delete[] buffer;
}


void Cartridge::printInfo()
{
    fmt::print("Game Title: {}\nCartridge Type: {:#x}\n", this->gameTitle, this->cartridgeType);
    fmt::print("ROM size: {}\nRAM size: {}\n", this->romSize, this->ramSize);
}


bool Cartridge::checksum(u8* buffer)
{
    u8 checksum = 0x19;
    for(u16 addr = 0x0134; addr <= 0x014d; addr++)
        checksum += buffer[addr];

    if(checksum != 0)
    {
        throw std::runtime_error(
            fmt::format("Cartridge header checksum failed. File '{}' is probably not a valid Game Boy ROM.", this->fileName)
        );
    }

    fmt::print("Checksum: PASSED\n");
    return true;
}


void Cartridge::initializeMBC(std::ifstream *f)
{
    int romSizeInBytes = 0x8000 * (1 << this->romSize);
    int ramSizeInBytes = 0;
    switch (this->ramSize) {
        case 0x0:
            ramSizeInBytes = 0;
            break;

        case 0x2:
            ramSizeInBytes = 0x2000;
            break;

        case 0x3:
            ramSizeInBytes = 0x8000;
            break;

        case 0x4:
            ramSizeInBytes = 0x20000;
            break;

        case 0x5:
            ramSizeInBytes = 0x10000;
            break;
    }

    switch (cartridgeType) {
        case 0x0:
            this->mbc = new NoMBC(romSizeInBytes, ramSizeInBytes);
            break;

        default:
            fmt::print(stderr, "Error, emulator does not support cartridge type {:#x}\n", cartridgeType);
            exit(EXIT_FAILURE);
            break;
    }

    this->mbc->loadROM(f);
}
