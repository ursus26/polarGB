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

#include <cassert>
#include <fmt/format.h>
#include "polarGB/register.h"


Register::Register()
{
    /* Initialize the registers. */
    writeDouble(RegID_AF, 0x01b0);
    writeDouble(RegID_BC, 0x0013);
    writeDouble(RegID_DE, 0x00d8);
    writeDouble(RegID_HL, 0x014d);
    this->SP = 0xfffe;
    this->PC = 0x0100;
}


/**
 * Reads the content of a register.
 */
u16 Register::read(regID_t id)
{
    u16 out = 0;

    switch(id) {
        case RegID_NONE:
            assert(false);
            break;

        /* Single registers. */
        case RegID_A:
        case RegID_F:
        case RegID_B:
        case RegID_C:
        case RegID_D:
        case RegID_E:
        case RegID_H:
        case RegID_L:
            out = readSingle(id);
            break;

        /* Double sized registers. */
        case RegID_AF:
        case RegID_BC:
        case RegID_DE:
        case RegID_HL:
        case RegID_SP:
        case RegID_PC:
            out = readDouble(id);
            break;

        default:
            assert(false);
            break;
    }
    return out;
}


/**
 * Writes a value to a register. Note there are restrictions. A 1 byte register can only store 1
 * byte, not the full 2 byte (u16 type) value argument.
 */
void Register::write(regID_t id, u16 value)
{
    switch(id) {
        case RegID_NONE:
            assert(false);
            break;

        /* Single registers. */
        case RegID_A:
        case RegID_F:
        case RegID_B:
        case RegID_C:
        case RegID_D:
        case RegID_E:
        case RegID_H:
        case RegID_L:
            assert(value < 0x100);
            writeSingle(id, (u8)(value & 0xff));
            break;

        /* Double sized registers. */
        case RegID_AF:
        case RegID_BC:
        case RegID_DE:
        case RegID_HL:
        case RegID_SP:
        case RegID_PC:
            writeDouble(id, value);
            break;

        default:
            assert(false);
            break;
    }
}


/**
 * Writes a byte to a register specified by its id. We can only write to A, B, C, D, E, H and L.
 */
void Register::writeSingle(regID_t id, u8 val)
{
    switch(id)
    {
        case RegID_A:
            AF.high = val;
            break;
        case RegID_F:
            AF.low = val & 0xf0;
            break;
        case RegID_B:
            BC.high = val;
            break;
        case RegID_C:
            BC.low = val;
            break;
        case RegID_D:
            DE.high = val;
            break;
        case RegID_E:
            DE.low = val;
            break;
        case RegID_H:
            HL.high = val;
            break;
        case RegID_L:
            HL.low = val;
            break;
        default:
            assert(false);
            break;
    }
}


/**
 * Writes a word to a register specified by its id. We can only write to the register pairs:
 * AF, BC, DE and HL.
 */
void Register::writeDouble(regID_t id, u16 val)
{
    switch(id)
    {
        case RegID_AF:
            writeSingle(RegID_A, (val >> 8) & 0xff);
            AF.low = 0xf0 & val;
            break;
        case RegID_BC:
            writeSingle(RegID_B, (val >> 8) & 0xff);
            writeSingle(RegID_C, val & 0xff);
            break;
        case RegID_DE:
            writeSingle(RegID_D, (val >> 8) & 0xff);
            writeSingle(RegID_E, val & 0xff);
            break;
        case RegID_HL:
            writeSingle(RegID_H, (val >> 8) & 0xff);
            writeSingle(RegID_L, val & 0xff);
            break;
        case RegID_SP:
            SP = val;
            break;
        case RegID_PC:
            PC = val;
            break;
        default:
            assert(false);
            break;
    }
}


u8 Register::readSingle(regID_t id)
{
    switch(id)
    {
        case RegID_A:
            return AF.high;
        case RegID_F:
            return AF.low & 0xf0;
        case RegID_B:
            return BC.high;
        case RegID_C:
            return BC.low;
        case RegID_D:
            return DE.high;
        case RegID_E:
            return DE.low;
        case RegID_H:
            return HL.high;
        case RegID_L:
            return HL.low;
        default:
            assert(false);
            break;
    }
    return 0;
}


u16 Register::readDouble(regID_t id)
{
    u16 out = 0;
    switch(id)
    {
        case RegID_AF:
            out = (readSingle(RegID_A) << 8) + (AF.low & 0xf0);
            break;
        case RegID_BC:
            out = (readSingle(RegID_B) << 8) + readSingle(RegID_C);
            break;
        case RegID_DE:
            out = (readSingle(RegID_D) << 8) + readSingle(RegID_E);
            break;
        case RegID_HL:
            out = (readSingle(RegID_H) << 8) + readSingle(RegID_L);
            break;
        case RegID_SP:
            out = SP;
            break;
        case RegID_PC:
            out = PC;
            break;
        default:
            assert(false);
            break;
    }
    return out;
}


bool Register::getFlagZero() const
{
    return getBitFromFlags(7);
}


bool Register::getFlagSub() const
{
    return getBitFromFlags(6);
}


bool Register::getFlagHalfCarry() const
{
    return getBitFromFlags(5);
}


bool Register::getFlagCarry() const
{
    return getBitFromFlags(4);
}


void Register::setFlagZero(bool val)
{
    setBitFromFlags(7, val);
}


void Register::setFlagSub(bool val)
{
    setBitFromFlags(6, val);
}


void Register::setFlagHalfCarry(bool val)
{
    setBitFromFlags(5, val);
}


void Register::setFlagCarry(bool val)
{
    setBitFromFlags(4, val);
}


/**
 * Return the bit value of a flag in the flags register.
 */
bool Register::getBitFromFlags(int position) const
{
    assert(position >= 0);
    assert(position < 8);
    return (this->AF.low >> position) & 0x1;
}


void Register::setBitFromFlags(int position, bool val)
{
    assert(position >= 0);
    assert(position < 8);

    bool currentVal = getBitFromFlags(position);

    /* If the current value is not the desired value then flip the bit. Otherwise do nothing. */
    if(currentVal != val)
        this->AF.low ^= (1 << position);
}


void Register::printRegister()
{
    fmt::print("AF: {:#x}\n", readDouble(RegID_AF));
    fmt::print("BC: {:#x}\n", readDouble(RegID_BC));
    fmt::print("DE: {:#x}\n", readDouble(RegID_DE));
    fmt::print("HL: {:#x}\n", readDouble(RegID_HL));
}
