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

#ifndef MBC_H
#define MBC_H

#include <fstream>
#include <vector>
#include "types.h"

class MBC
{
public:
    MBC(int romSizeInBytes, int ramSizeInBytes);
    virtual ~MBC();

    void loadROM(std::ifstream *f);
    void clear();

    virtual u8 read(u16 address);
    virtual void write(u16 address, u8 data);

protected:
    int romSize;
    std::vector<u8> romMem;

    int ramSize;
    std::vector<u8> ramMem;
};

class NoMBC : public MBC
{
public:
    NoMBC(int romSizeInBytes, int ramSizeInBytes);
    ~NoMBC();
};

#endif /* MBC_H */
