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

#include <fstream>
#include <string>
#include "types.h"

class Cartridge
{
public:
    Cartridge();
    ~Cartridge();

    void load(std::string fileName); /* Can throw a runtime_error. */
    void printInfo();

    u8 read(u16 address);
    void write(u16 address, u8 data);

private:
    std::string fileName;
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

    void processCartridgeHeader();
    bool checksum();
    unsigned int getFileSize(std::ifstream *f);
    void loadFile(std::string fileName);
    void removeLoadedCartridge();
};

#endif /* CARTRIDGE_H */
