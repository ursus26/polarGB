#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include "types.h"

class Cartridge
{
public:
    Cartridge();
    ~Cartridge();

    bool loadCartridge(std::string fileName);
    void processCartridgeHeader();
    void printInfo();
    bool checksum();

    uint8_t read(unsigned int address);

private:
    U8* mem;
    unsigned int size;

    /* Cartridge info taken from the cartridge header. Memory location 0x100-0x150 */
    std::string gameTitle;
    int CGBFlag;
    bool SGBFlag;
    int cartridgeType;
    int romSize;
    int ramSize;
    int destinationCode;
};

#endif /* CARTRIDGE_H */
