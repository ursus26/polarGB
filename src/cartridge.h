#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>

class Cartridge
{
public:
    Cartridge();
    ~Cartridge();

    bool loadCartridge(std::string fileName);
    uint8_t load(unsigned int address);

private:
    uint8_t* mem;
    unsigned int size;
};

#endif /* CARTRIDGE_H */
