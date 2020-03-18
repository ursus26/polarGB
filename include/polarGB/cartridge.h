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

#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include "types.h"

class Cartridge
{
public:
    Cartridge();
    ~Cartridge();

    void startUp();
    void shutDown();

    bool loadCartridge(std::string fileName);
    void processCartridgeHeader();
    void printInfo();
    bool checksum();

    uint8_t read(unsigned int address);

private:
    u8* mem;
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
