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
#include <iostream>
#include <fstream>
#include "polarGB/cartridge.h"


using namespace std;


Cartridge::Cartridge()
{
}

Cartridge::~Cartridge()
{
}


void Cartridge::startUp()
{
    this->mem = nullptr;
    this->size = 0;
}


void Cartridge::shutDown()
{
    delete[] this->mem;
    this->mem = nullptr;
    this->size = 0;
}


bool Cartridge::loadCartridge(string fileName)
{
    cout << "Loading a new cartridge from file: " << fileName << endl;

    /* Open the file that acts as a gameboy cartridge. */
    ifstream f;
    f.open(fileName, ios::in | ios::binary);

    if(!f.is_open())
    {
        cout << "Error, could not open \"" << fileName << "\"" << endl;
        exit(EXIT_FAILURE);
    }

    if(f.good())
    {
        /* Get the length of the file. */
        f.seekg (0, f.end);
        this->size = f.tellg();
        f.seekg (0, f.beg);
    }

    // cout << "File size: 0x" << hex << this->size << dec << endl;

    /* Read the cartridge into a buffer. */
    this->mem = new uint8_t[this->size];
    for(unsigned int i = 0; i < this->size && f.good(); i++)
        this->mem[i] = (uint8_t) f.get();

    /* Close the file. */
    f.close();

    processCartridgeHeader();

    return true;
}


u8 Cartridge::read(u16 address)
{
    if(address > this->size || this->mem == nullptr)
        return 0;

    return mem[address];
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
    cout << "Game Title: " << this->gameTitle << endl;
    cout << "Cartridge Type: " << hex << cartridgeType << endl;
}


bool Cartridge::checksum()
{
    u8 checksum = 0x19;
    for(u16 addr = 0x0134; addr <= 0x014d; addr++)
        checksum += this->read(addr);

    if(checksum != 0)
    {
        std::cerr << "Error, checksum on boot failed. Possibly did not use a correct cartridge." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Checksum: PASSED" << std::endl;
    return true;
}
