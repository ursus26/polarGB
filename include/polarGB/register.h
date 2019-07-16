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

#ifndef REGISTER_H
#define REGISTER_H

#include "types.h"


/* Decleration of the standard Gameboy registers that can store 2 variables of 8 bits or one 16
 * bits variable.
 *
 * Register map:
 *
 * 0        8        16 bits
 * -------------------
 * |   A    |   F    |
 * -------------------
 * |   B    |   C    |
 * -------------------
 * |   D    |   E    |
 * -------------------
 * |   H    |   L    |
 * -------------------
 * |  stack pointer  |
 * -------------------
 * | program counter |
 * -------------------
 */


const int IDX_HIGH = 0;
const int IDX_LOW = 1;


typedef enum RegID
{
    RegID_A,
    RegID_F,
    RegID_B,
    RegID_C,
    RegID_D,
    RegID_E,
    RegID_H,
    RegID_L,
    RegID_AF,
    RegID_BC,
    RegID_DE,
    RegID_HL,
    RegID_SP,
    RegID_PC,
    RegID_MISS  /* Used if a value is not in a register but in memory. */
} RegID;


union RegPair
{
    u8 single[2];
    u16 pair;
};


class Register
{
public:
    Register();

    /* Functions for writing to a register or copying. */
    void write8(RegID id, u8 val);
    void write16(RegID id, u16 val);
    void copy8(RegID dest, RegID src);

    /* Functions for loading data from a register. */
    u8 read8(RegID id);
    u16 read16(RegID id);

    /* Operations on the program counter. */
    u16 getProgramCounter() const;
    void setProgramCounter(u16 val);
    void incProgramCounter();

    /* Operations on the stack pointer. */
    u16 getStackPointer() const;
    void incStackPointer();
    void decStackPointer();

    /* Functions for getting the flags from register F. */
    bool getFlagZero() const;
    bool getFlagSub() const;
    bool getFlagHalfCarry() const;
    bool getFlagCarry() const;

    /* Functions for setting the flags from register F. */
    void setFlagZero(bool val);
    void setFlagSub(bool val);
    void setFlagHalfCarry(bool val);
    void setFlagCarry(bool val);

private:
    /* Bit flag operations. */
    bool getBitFromFlags(int position) const;
    void setBitFromFlags(int position, bool val);

    /* Register variables. */
    RegPair AF;
    RegPair BC;
    RegPair DE;
    RegPair HL;
    u16 SP;
    u16 PC;
};


#endif /* REGISTER_H */
