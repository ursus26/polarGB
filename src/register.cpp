#include "register.h"
#include <stdexcept>
#include <stdio.h>


Register::Register()
{
    /* Initialize the registers to 0. */
    this->AF.pair = 0;
    this->BC.pair = 0;
    this->DE.pair = 0;
    this->HL.pair = 0;
    this->SP = 0xfffe;
    this->PC = 0;
}


/**
 * Writes a byte to a register specified by its id. We can only write to A, B, C, D, E, H and L.
 */
void Register::write8(RegID id, u8 val)
{
    switch(id)
    {
        case RegID_A:
            AF.single[IDX_HIGH] = val;
            break;
        case RegID_B:
            BC.single[IDX_HIGH] = val;
            break;
        case RegID_C:
            BC.single[IDX_LOW] = val;
            break;
        case RegID_D:
            DE.single[IDX_HIGH] = val;
            break;
        case RegID_E:
            DE.single[IDX_LOW] = val;
            break;
        case RegID_H:
            HL.single[IDX_HIGH] = val;
            break;
        case RegID_L:
            HL.single[IDX_LOW] = val;
            break;
        default:
            throw std::invalid_argument("Cannot write a byte becasue register id was invalid.");
            break;
    }
}


/**
 * Writes a word to a register specified by its id. We can only write to the register pairs:
 * BC, DE and HL.
 */
void Register::write16(RegID id, u16 val)
{
    switch(id)
    {
        case RegID_AF:
            AF.pair = val;
            break;
        case RegID_BC:
            BC.pair = val;
            break;
        case RegID_DE:
            DE.pair = val;
            break;
        case RegID_HL:
            HL.pair = val;
            break;
        case RegID_SP:
            SP = val;
            break;
        default:
            throw std::invalid_argument("Cannot write a word becasue register id was invalid.");
            break;
    }
}

/**
 * Copies the value from register src to register dest.
 */
void Register::copy8(RegID dest, RegID src)
{
    u8 srcVal = read8(src);
    write8(dest, srcVal);
}


u8 Register::read8(RegID id)
{
    switch(id)
    {
        case RegID_A:
            return AF.single[IDX_HIGH];

        case RegID_B:
            return BC.single[IDX_HIGH];

        case RegID_C:
            return BC.single[IDX_LOW];

        case RegID_D:
            return DE.single[IDX_HIGH];

        case RegID_E:
            return DE.single[IDX_LOW];

        case RegID_H:
            return HL.single[IDX_HIGH];

        case RegID_L:
            return HL.single[IDX_LOW];

        default:
            throw std::invalid_argument("Cannot load byte from register.");
            break;
    }

    return 0;
}


u16 Register::read16(RegID id)
{
    switch(id)
    {
        case RegID_AF:
            return AF.pair;

        case RegID_BC:
            return BC.pair;

        case RegID_DE:
            return DE.pair;

        case RegID_HL:
            return HL.pair;

        case RegID_SP:
            return SP;

        case RegID_PC:
            return PC;

        default:
            throw std::invalid_argument("Cannot write a word becasue register id was invalid.");
            break;
    }

    return 0;
}


bool Register::getFlagZero()
{
    return getBitFromFlags(7);
}


bool Register::getFlagSub()
{
    return getBitFromFlags(6);
}


bool Register::getFlagHalfCarry()
{
    return getBitFromFlags(5);
}


bool Register::getFlagCarry()
{
    return getBitFromFlags(4);
}

/**
 * Return the bit value of a flag in the flags register.
 */
bool Register::getBitFromFlags(int position)
{
    return (this->AF.single[IDX_LOW] >> position) & 0x1;
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
        this->AF.single[IDX_LOW] ^= (1 << position);

        // reg.regAF.single.Lo = reg.regAF.single.Lo ^ (1 << position);
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
u16 Register::getStackPointer()
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
