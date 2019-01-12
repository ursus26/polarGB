#include "cartridge.h"
#include <iostream>
#include <fstream>

using namespace std;

Cartridge::Cartridge()
{
    this->mem = nullptr;
    this->size = 0;
}

Cartridge::~Cartridge()
{
    delete[] this->mem;
    this->mem = nullptr;
    this->size = 0;
}

bool Cartridge::loadCartridge(string fileName)
{
    cout << "Cartridge::loadCartridge | Loading a new cartridge from file: " << fileName << endl;

    /* Open the file that acts as a gameboy cartridge. */
    ifstream f;
    f.open(fileName, ios::in | ios::binary);

    /* Get the length of the file. */
    f.seekg (0, f.end);
    this->size = f.tellg();
    f.seekg (0, f.beg);
    cout << "File size: 0x" << hex << this->size << dec << endl;

    /* Read the cartridge into a buffer. */
    this->mem = new uint8_t[this->size];
    for(unsigned int i = 0; i < this->size; i++)
        this->mem[i] = (uint8_t) f.get();

    /* Close the file. */
    f.close();

    processCartridgeHeader();

    return true;
}


uint8_t Cartridge::read(unsigned int address)
{
    if(address > size)
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
        U8 character = this->read(i);
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
}


void Cartridge::printInfo()
{
    cout << "Game Title: " << this->gameTitle << endl;
    cout << "Cartridge Type: " << hex << cartridgeType << endl;
}
