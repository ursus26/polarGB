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
    cout << "File size: " << this->size << endl;

    /* Read the cartridge into a buffer. */
    this->mem = new uint8_t[this->size];
    for(unsigned int i = 0; i < this->size; i++)
        this->mem[i] = (uint8_t) f.get();

    /* Close the file. */
    f.close();

    /* Print the result. */
    // for(unsigned int i = 0; i < this->size; i++)
    // {
    //     cout << hex << (int) this->mem[i] << " ";
    // }
    // cout << endl;


    return true;
}


uint8_t Cartridge::load(unsigned int address)
{
    if(address > size)
        return 0;

    return mem[address];
}
