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
#include <stdio.h>
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
 * Writes a byte to a register specified by its id. We can only write to A, B, C, D, E, H and L.
 */
void Register::writeSingle(RegID id, u8 val)
{
    switch(id)
    {
        case RegID_A:
            AF.high = val;
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
            throw std::invalid_argument("Cannot write a byte becasue register id was invalid.");
            break;
    }
}


/**
 * Writes a word to a register specified by its id. We can only write to the register pairs:
 * AF, BC, DE and HL.
 */
void Register::writeDouble(RegID id, u16 val)
{
    switch(id)
    {
        case RegID_AF:
            writeSingle(RegID_A, (val >> 8) & 0xff);
            AF.low = 0xff & val;
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
            throw std::invalid_argument("Cannot write a word becasue register id was invalid.");
            break;
    }
}

/**
 * Copies the value from register src to register dest.
 */
void Register::copySingle(RegID dest, RegID src)
{
    u8 srcVal = readSingle(src);
    writeSingle(dest, srcVal);
}


u8 Register::readSingle(RegID id)
{
    switch(id)
    {
        case RegID_A:
            return AF.high;

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
            throw std::invalid_argument("Cannot load byte from register.");
            break;
    }

    return 0;
}


u16 Register::readDouble(RegID id)
{
    u16 returnValue = 0x0;
    switch(id)
    {
        case RegID_AF:
            returnValue = (readSingle(RegID_A) << 8) + AF.low;
            break;

        case RegID_BC:
            returnValue = (readSingle(RegID_B) << 8) + readSingle(RegID_C);
            break;

        case RegID_DE:
            returnValue = (readSingle(RegID_D) << 8) + readSingle(RegID_E);
            break;

        case RegID_HL:
            returnValue = (readSingle(RegID_H) << 8) + readSingle(RegID_L);
            break;

        case RegID_SP:
            returnValue = SP;
            break;

        case RegID_PC:
            returnValue = PC;
            break;

        default:
            throw std::invalid_argument("Cannot write a word becasue register id was invalid.");
            break;
    }

    return returnValue;
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

/**
 * Return the bit value of a flag in the flags register.
 */
bool Register::getBitFromFlags(int position) const
{
    return (this->AF.low >> position) & 0x1;
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


void Register::setBitFromFlags(int position, bool val)
{
    bool currentVal = getBitFromFlags(position);

    /* If the current value is not the desired value then flip the bit. Otherwise do nothing. */
    if(currentVal != val)
        this->AF.low ^= (1 << position);
}


/**
 * Returns the value of the program counter.
 */
u16 Register::getProgramCounter() const
{
    return this->PC;
}


/**
 * Changes the value of the program counter to a new value.
 */
void Register::setProgramCounter(u16 val)
{
    this->PC = val;
}


/**
 * Increases the program counter by 1.
 */
void Register::incProgramCounter()
{
    this->PC++;
}


/**
 * Returns the value of the stack pointer.
 */
u16 Register::getStackPointer() const
{
    return this->SP;
}


/**
 * Increases the stack pointer by 1.
 */
void Register::incStackPointer()
{
    this->SP++;
}


/**
 * Decrease the stack pointer by 1.
 */
void Register::decStackPointer()
{
    this->SP--;
}


void Register::printRegister()
{
    printf("AF: 0x%04x\n", readDouble(RegID_AF));
    printf("BC: 0x%04x\n", readDouble(RegID_BC));
    printf("DE: 0x%04x\n", readDouble(RegID_DE));
    printf("HL: 0x%04x\n", readDouble(RegID_HL));
}
