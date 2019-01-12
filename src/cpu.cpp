#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "log.h"


Cpu::Cpu(Mmu* m)
{
    std::cout << "Initialize CPU" << std::endl;

    /* Initialize the memory manager. */
    mmu = m;
    isRunning = true;
    cyclesCompleted = 0;
}

Cpu::~Cpu()
{
    /* Remove the reference to the memory manager but don't delete it since this object did not
     * create the memory manager. */
    mmu = nullptr;
}


void Cpu::boot()
{
    /* Initialize the registers. */
    this->reg.write16(RegID_AF, 0x01b0);    /* Initialize AF register. */
    this->reg.write16(RegID_BC, 0x0013);    /* Initialize BC register. */
    this->reg.write16(RegID_DE, 0x00d8);    /* Initialize DE register. */
    this->reg.write16(RegID_HL, 0x014d);    /* Initialize HL register. */
    this->reg.write16(RegID_SP, 0xfffe);    /* Initialize the stack. */

    /* Initialize the memory. */
    this->mmu->boot();

    /* Perform checksum. */
    U8 checksum = 0x19;
    for(U16 addr = 0x0134; addr <= 0x014d; addr++)
        checksum += this->mmu->read(addr);

    if(checksum != 0)
    {
        std::cerr << "Error, checksum on boot failed. Possibly did not use a correct cartridge." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Checksum: PASSED" << std::endl;

    /* Finish the boot sequence by setting the pc counter to 0x100. */
    this->reg.setProgramCounter(0x100);
}

void Cpu::run(int cycles)
{
    for(int i = 0; i < cycles; i++)
    {
        /* Fetch the next instruction. */
        U8 opcode = fetchNextInstruction();

        /* Execute the instruction. */
        executeInstruction(opcode);
    }
}


void Cpu::runSingleFrame()
{
    while(cyclesCompleted < MAX_INSTRUCTIONS_PER_FRAME)
    {
        /* Fetch the next instruction. */
        U8 opcode = fetchNextInstruction();

        /* Execute the instruction. */
        cyclesCompleted += executeInstruction(opcode);
    }

    cyclesCompleted -= MAX_INSTRUCTIONS_PER_FRAME;
}


/**
 * This function fetches the next instruction from memory pointed by the program counter. Then it
 * increments the program counter and returns the opcode.
 */
U8 Cpu::fetchNextInstruction()
{
    /* Get the program counter. */
    U16 pc = reg.getProgramCounter();
    printf("PC: 0x%04x\n", pc);

    /* Fetch the next instruction from memory. */
    U8 opcode = mmu->read(pc);

    /* Increase the program counter by 1 and return the instruction code. */
    reg.incProgramCounter();
    return opcode;
}

/**
 * Fetches 16 bits from memory by stitching 2 bytes together. The first byte is the low byte and
 * the second byte is the high byte.
 */
U16 Cpu::fetchNext16Bits()
{
    /* Get the low and high U8 of the 16 bit address. */
    U16 low = fetchNextInstruction();
    U16 high = fetchNextInstruction();

    /* Construct the address. */
    U16 word = (high << 8) + low;

    return word;
}

/**
 * Read some data from memory specified by an address in a register.
 */
U8 Cpu::readMem(RegID memPointer)
{
    /* Get the memory location. */
    U16 memLocation = reg.read16(memPointer);

    /* Get and return the data from memory. */
    return (mmu->read(memLocation));
}


/**
 * Writes some data to memory specified by an address in a register.
 */
void Cpu::writeMem(RegID memPointer, U8 data)
{
    /* Get the memory location. */
    U16 memLocation = reg.read16(memPointer);

    /* Get and return the data from memory. */
    mmu->write(memLocation, data);
}


/**
 * Writes a value to a memory address.
 */
void Cpu::executeLD8(U16 addr, U8 val)
{
    mmu->write(addr, val);
}


/**
 * Fetches 16 bits pointed by the program counter and stores it in a register pair.
 */
void Cpu::executeLD16(RegID dest)
{
    reg.write16(dest, fetchNext16Bits());
}


/**
 * Pushes the contents of a register pair to the stack.
 */
void Cpu::executePUSH(U16 val)
{
    /* Get the high and low byte. */
    U8 low = val & 0xff;
    U8 high = (val >> 8) & 0xff;
    U16 sp = reg.getStackPointer();

    /* Write the value to the stack. */
    mmu->write(sp - 1, high);
    mmu->write(sp - 2, low);

    /* Decrease the stack pointer by two. */
    reg.decStackPointer();
    reg.decStackPointer();
}


/**
 * Pops 2 bytes from the stack.
 */
U16 Cpu::executePOP()
{
    /* Get 2 bytes from the stack. */
    U8 val = mmu->readU16(reg.getStackPointer());

    /* Increases the stack pointer by two. */
    reg.incStackPointer();
    reg.incStackPointer();

    return val;
}


/**
 * Adds a value to the contents of register A and store it in register A.
 */
void Cpu::executeADD8(U8 val)
{
    /* Add the value of register 2 to register 1. */
    U8 result = reg.read8(RegID_A) + val;
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(false);               /* Always reset the Sub flag. */
    reg.setFlagHalfCarry(halfCarryTest(val1, val2));
    reg.setFlagCarry(carryTest(val1, val2));

    /* Store the addition. */
    reg.write8(RegID_A, result);
}

/**
 * Adds a value and the carry bit to the contents of register A and stores it in register A.
 */
void Cpu::executeADC(U8 val)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;

    U8 carry;
    if(reg.getFlagCarry())
        carry = 1;
    else
        carry = 0;

    U8 result = reg.read8(RegID_A) + val + carry;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(false);               /* Always reset the Sub flag. */
    reg.setFlagHalfCarry(halfCarryTest(val1, val2 + carry));
    reg.setFlagCarry(carryTest(val1, val2 + carry));

    /* Store the addition. */
    reg.write8(RegID_A, result);
}

/**
 * Subtracts a value from register A.
 */
void Cpu::executeSUB(U8 val)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;
    U8 result = reg.read8(RegID_A) - val;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true);               /* Always set the Sub flag. */
    reg.setFlagHalfCarry(halfBorrowTest(val1, val2));
    reg.setFlagCarry(borrowTest(val1, val2));

    /* Store the addition. */
    reg.write8(RegID_A, result);
}

/**
 * Subtracts a value and the carry bit from register A.
 */
void Cpu::executeSBC(U8 val)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;

    U8 carry;
    if(reg.getFlagCarry())
        carry = 1;
    else
        carry = 0;

    U8 result = reg.read8(RegID_A) - val - carry;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true);               /* Always set the Sub flag. */
    reg.setFlagHalfCarry(halfBorrowTest(val1, val2 - carry));
    reg.setFlagCarry(borrowTest(val1, val2 - carry));

    /* Store the addition. */
    reg.write8(RegID_A, result);
}


/**
 * Bitwise AND of a value with register A. Then store the result in register A.
 */
void Cpu::executeAND(U8 val)
{
    /* Bitwise AND. */
    U8 result = reg.read8(RegID_A) & val;

    /* Set or reset the flags. */
    reg.setFlagZero(reg.read8(RegID_A) == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(true);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write8(RegID_A, result);
}


/**
 * Bitwise XOR of a value with register A. Then store the result in register A.
 */
void Cpu::executeXOR(U8 val)
{
    /* Bitwise xor. */
    U8 result = reg.read8(RegID_A) ^ val;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write8(RegID_A, result);
}


/**
 * Bitwise OR of a value with register A. Then store the result in register A.
 */
void Cpu::executeOR(U8 val)
{
    /* Bitwise or. */
    U8 result = reg.read8(RegID_A) | val;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write8(RegID_A, result);
}


/**
 * Execute the CP instruction and set the flags based on the result.
 */
void Cpu::executeCP(U8 val)
{
    U8 src = reg.read8(RegID_A);

    reg.setFlagZero(src == val);
    reg.setFlagHalfCarry(src > val);
    reg.setFlagCarry(src < val);
    reg.setFlagSub(true);
}


U8 Cpu::executeINC8(U8 val)
{
    U8 result = val + 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(halfCarryTest(val, 1));

    return result;
}


U8 Cpu::executeDEC8(U8 val)
{
    U8 result = val - 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(true);
    reg.setFlagHalfCarry(halfBorrowTest(val, 1));

    /* Store the result. */
    return result;
}


void Cpu::executeINC16(RegID dest)
{
    U16 result = reg.read16(dest) + 1;
    reg.write16(dest, result);
}


void Cpu::executeDEC16(RegID dest)
{
    U16 result = reg.read16(dest) - 1;
    reg.write16(dest, result);
}


void Cpu::executeADD16(RegID dest, int val)
{
    /* Get the values from the registers and do the addition. */
    U16 destVal = reg.read16(dest);
    U16 result = val + destVal;

    /* Set the flags. */
    reg.setFlagSub(false);
    if(val < 0)
    {
        reg.setFlagHalfCarry(halfBorrowTest(destVal, val));
        reg.setFlagCarry(borrowTest(destVal, val));
    }
    else
    {
        reg.setFlagHalfCarry(halfCarryTest(destVal, val));
        reg.setFlagCarry(carryTest(destVal, val));
    }
    if(dest == RegID_SP)
        reg.setFlagZero(false);

    /* Store the result. */
    reg.write16(dest, result);
}


int Cpu::executeJP(int conditionFlag)
{
    int cycles = 3;
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();

    /* Code execution for: JP (HL) */
    if(conditionFlag == COND_HL)
    {
        reg.setProgramCounter(reg.read16(RegID_HL));
        return 1;
    }

    /* Get the jump location and check if the condition is true or does not exist. */
    U16 jumpLocation = fetchNext16Bits();
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        printf("PC <- 0x%04x\n", jumpLocation);
        reg.setProgramCounter(jumpLocation);
        cycles = 4;
    }

    return cycles;
}


int Cpu::executeJR(int conditionFlag)
{
    int cycles = 2;
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    U8 tmp = fetchNextInstruction();
    I8 step = (static_cast<I8> (tmp));

    int a = (int) tmp;
    int b = (int) step;

    // TODO: REMOVE
    std::cout << "==> JR Step: " << b << ", Original: " << a << " Zero: " << zero << std::endl;

    /* Get the jump location and check if the condition is true or does not exist. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        U16 newPC = reg.getProgramCounter() + step;
        reg.setProgramCounter(newPC);
        cycles = 3;
    }

    return cycles;
}


/**
 * Push the program counter on the stack and adjust the program counter.
 */
int Cpu::executeCALL(U16 jumpLocation, int conditionFlag)
{
    int cycles = 3;
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    U16 pc = reg.getProgramCounter();

    /* Write the program counter to the stack if the condition matches. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        this->executePUSH(pc);
        cycles = 6;

        /* Change the program counter. */
        reg.setProgramCounter(jumpLocation);
    }

    return cycles;
}


/**
 * Push the program counter on the stack and adjust the program counter.
 */
int Cpu::executeRET(int conditionFlag)
{
    int cycles = 2;
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();

    /* Write the program counter to the stack if the condition matches. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        U16 val = this->executePOP();
        reg.setProgramCounter(val);
        cycles = 4;

        /* Adjust the stack pointer. */
        reg.incStackPointer();
        reg.incStackPointer();
    }

    if(conditionFlag != COND_NONE)
        cycles = 5;

    return cycles;
}


/**
 * Set a specific bit in a byte to 1.
 */
U8 Cpu::executeSET(U8 src, int bitNumber)
{
    return (src | (1 <<bitNumber));
}


/**
 * Set a specific bit in a byte to 0.
 */
U8 Cpu::executeRES(U8 src, int bitNumber)
{
    return (src & ~(1 << bitNumber));
}


/**
 * Get the bit value of a specific bit in a byte and set the zero flag as the complement of
 * this bit.
 */
void Cpu::executeBIT(U8 src, int bitNumber)
{
    bool bitValue = (src >> bitNumber) & 0x1;

    reg.setFlagZero(!bitValue);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(true);
}


/**
 * Swaps the first 4 bits with the last 4 bits in a byte. For example 0xAF will result in 0xFA and
 * 0x10 will result in 0x1.
 */
U8 Cpu::executeSWAP(U8 src)
{
    U8 result = (src << 4) + (src >> 4);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    return result;
}


/**
 * Shifts the byte to the left. Note: Bit 7 goes to the carry flag and the carry flag to bit 0.
 */
U8 Cpu::executeRL(U8 src)
{
    U8 carry = (U8) reg.getFlagCarry();
    U8 result = (src << 1) | carry;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x80) == 0x80);

    return result;
}


/**
 * Shifts the byte to the right. Note: Bit 0 goes to the carry flag and the carry flag to bit 7.
 */
U8 Cpu::executeRR(U8 src)
{
    U8 carry = ((U8) reg.getFlagCarry()) << 7;
    U8 result = (src >> 1) | carry;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x1) == 0x1);

    return result;
}


/**
 * Shifts the byte to the left. Note: Bit 7 goes to both the carry flag and bit 0.
 */
U8 Cpu::executeRLC(U8 src)
{
    U8 result = (src << 1) | (src >> 7);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x80) == 0x80);

    return result;
}


/**
 * Shifts the byte to the right. Note: Bit 0 goes to both the carry flag and bit 7.
 */
U8 Cpu::executeRRC(U8 src)
{
    U8 result = (src << 7) | (src >> 1);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x1) == 0x1);

    return result;
}


/**
 * Execute RL with register A and set the zero flag to false.
 */
void Cpu::executeRLA()
{
    U8 src = reg.read8(RegID_A);
    U8 result = executeRL(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite RR with register A and set the zero flag to false.
 */
void Cpu::executeRRA()
{
    U8 src = reg.read8(RegID_A);
    U8 result = executeRR(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite RLC with register A and set the zero flag to false.
 */
void Cpu::executeRLCA()
{
    U8 src = reg.read8(RegID_A);
    U8 result = executeRLC(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite RRC with register A and set the zero flag to false.
 */
void Cpu::executeRRCA()
{
    U8 src = reg.read8(RegID_A);
    U8 result = executeRRC(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite SLA on a byte. It shifts the contents 1 bit to the left and bit 7 is stored in the
 * carry flag.
 */
U8 Cpu::executeSLA(U8 src)
{
    bool carry = (src & 0x80) == 0x80;
    U8 result = src << 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(carry);

    return result;
}


/**
 * Execite SRA on a byte. It shifts the contents 1 bit to the right. Bit 7 goes to itself and
 * bit 6 and bit 0 goes to the carry flag.
 */
U8 Cpu::executeSRA(U8 src)
{
    bool carry = (src & 0x1) == 0x1;
    U8 result = (src & 0x80) | (src >> 1);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(carry);

    return result;
}


/**
 * Execite SRA on a byte. It shifts the contents 1 bit to the right. Bit 7 goes to itself and
 * bit 6 and bit 0 goes to the carry flag.
 */
U8 Cpu::executeSRL(U8 src)
{
    bool carry = (src & 0x1) == 0x1;
    U8 result = src >> 1;;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(carry);

    return result;
}


bool Cpu::halfCarryTest(int val1, int val2)
{
    int test = ((val1 & 0xf) + (val2 & 0xf)) & 0x1;
    return (test == 0x1);
}


bool Cpu::halfBorrowTest(int val1, int val2)
{
    int test = ((val1 & 0x1f) - (val2 & 0x1f)) & 0x8;
    return (test == 0x8);
}


bool Cpu::carryTest(int val1, int val2)
{
    int test = ((val1 & 0xff) + (val2 & 0xff)) & 0x10;
    return (test == 0x10);
}


bool Cpu::borrowTest(int val1, int val2)
{
    int test = ((val1 & 0x1ff) + (val2 & 0x1ff)) & 0x80;
    return (test == 0x80);
}
