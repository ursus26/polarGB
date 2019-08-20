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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "polarGB/cpu.h"
#include "polarGB/log.h"


using namespace std;

Cpu::Cpu(Mmu* m, Video* vid)
{
    /* Initialize the memory manager. */
    this->mmu = m;
    this->video = vid;
    this->isRunning = true;
    this->cyclesCompleted = 0;
    this->interruptController.initialise(this->mmu);
}

Cpu::~Cpu()
{
    /* Remove the reference to the memory manager but don't delete it since this object did not
     * create the memory manager. */
    mmu = nullptr;
}


void Cpu::boot()
{
    /* Initialise the registers. */
    this->reg.write16(RegID_AF, 0x01b0);    /* Initialise AF register. */
    this->reg.write16(RegID_BC, 0x0013);    /* Initialise BC register. */
    this->reg.write16(RegID_DE, 0x00d8);    /* Initialise DE register. */
    this->reg.write16(RegID_HL, 0x014d);    /* Initialise HL register. */
    this->reg.write16(RegID_SP, 0xfffe);    /* Initialise the stack. */

    /* Initialise the memory. */
    this->mmu->boot();

    /* Initialise the interrupt controller. */
    this->interruptController.disableInterrupts();

    /* Finish the boot sequence by setting the pc counter to 0x100. */
    this->reg.setProgramCounter(0x100);
    cout << endl;
}


void Cpu::run()
{
    chrono::time_point<chrono::high_resolution_clock> start, end;
    chrono::duration<double> elapsed_time;

    /* Delta time takes into account that the elapsed_time > FRAME_TIME. If we reset the start time
     * of the frame we can loose a little bit of time. This variable calculates this difference
     * and helps make our timing function more accurate. */
    double delta_time = 0.0;
    start = chrono::high_resolution_clock::now();

    while(true)
    {
        /* Measure the elapsed time. */
        end = chrono::high_resolution_clock::now();
        elapsed_time = end - start;

        if(elapsed_time.count() + delta_time >= FRAME_TIME)
        {
            /* Reset the timer and calculate the time we lost sleeping. */
            start = chrono::high_resolution_clock::now();
            delta_time = elapsed_time.count() + delta_time - FRAME_TIME;

            /* Run the instructions for the next frame. */
            runSingleFrame();

            /* Update display. */
            this->video->update();

            /* Check if we should close our window. */
            if(this->video->closeWindow())
                break;
        }
    }
}


void Cpu::runNCycles(int cycles)
{
    for(int i = 0; i < cycles; i++)
    {
        /* TESTING */
        Instruction* instr = fetchDecode();

        /* Increase the program counter. */
        reg.setProgramCounter(reg.getProgramCounter() + instr->instructionLength);

        /* Execute the instruction handler. */
        (this->*(instr->executionFunction))(instr);

        /* Print information about the instruction. */
        printInstructionInfo(instr);

        /* Instruction clean up. */
        delete instr;

        /* Fetch the next instruction. */
        // u8 opcode = fetchNextInstruction();
        //
        // /* Execute the instruction. */
        // executeInstruction(opcode);

    }
}


void Cpu::runSingleFrame()
{
    // u8 opcode = 0x0;
    while(cyclesCompleted < MAX_INSTRUCTIONS_PER_FRAME)
    {
        /* Check for interrupts before fetching the next instruction. This possibly changes the
         * program counter in order to execute a signal. */
        this->checkSignals();

        Instruction* instr = fetchDecode();

        /* Increase the program counter. */
        reg.setProgramCounter(reg.getProgramCounter() + instr->instructionLength);

        /* Execute the instruction handler. */
        (this->*(instr->executionFunction))(instr);

        /* Print information about the instruction. */
        printInstructionInfo(instr);

        /* Instruction clean up. */
        delete instr;



        // /* Fetch the next instruction. */
        // opcode = fetchNextInstruction();
        //
        // /* Execute the instruction. */
        // cyclesCompleted += executeInstruction(opcode);
    }

    cyclesCompleted -= MAX_INSTRUCTIONS_PER_FRAME;
}


Cpu::instruction_t * Cpu::fetchDecode()
{
    instruction_t* instr = new instruction_t;

    /* Get the program counter. */
    u16 pc = reg.getProgramCounter();

    /* Fetch the next instruction from memory. */
    u8 opcode = mmu->read(pc);

    instr->memoryLocation = pc;
    instr->opcode = opcode;
    decodeOpcode(instr, opcode);
    // instr->opcode = 0x0;

    return instr;
}


void Cpu::printInstructionInfo(instruction_t *instr)
{
    printf("%04x:\t", instr->memoryLocation);
    printf("%02x", instr->opcode);

    if(instr->instructionLength == 3)
    {
        u16 data = instr->operandSrc.immediate;
        printf(" %02x %02x    ", data & 0xff, data >> 8);
    }
    else if(instr->instructionLength == 2)
    {
        u16 data = instr->operandSrc.immediate;
        printf(" %02x         ", data & 0xff);
    }
    else
    {
        /* Padding for allignment. */
        printf("%-10s", " ");
    }

    printf("\t%s\n", instr->mnemonic);
}


u8 Cpu::loadOperand8bits(Operand* operand)
{
    switch(operand->type)
    {
        case OP_IMM:
            return operand->immediate;
        case OP_REG:
            return reg.read8(operand->reg);
        case OP_MEM:
            return readMem(operand->memPtr);
        case OP_NONE:
        default:
            return 0;
    }
}


u16 Cpu::loadOperand16bits(Operand* operand)
{
    switch(operand->type)
    {
        case OP_IMM:
            return operand->immediate;
        case OP_REG:
            return reg.read16(operand->reg);
        case OP_MEM:
            return readMem16bitsFromPointer(operand->memPtr);
        case OP_NONE:
        default:
            return 0;
    }
}


void Cpu::storeOperand8bits(Operand* operand, u8 value)
{
    switch(operand->type)
    {
        case OP_REG:
            reg.write8(operand->reg, value);
            break;
        case OP_MEM:
            writeMem(operand->memPtr, value);
            break;
        case OP_IMM:
        case OP_NONE:
        default:
            printf("Unexpected store 8 bits location, operand = %d\n", operand->type);
            break;
    }
}


void Cpu::storeOperand16bits(Operand* operand, u16 value)
{
    switch(operand->type)
    {
        case OP_REG:
            reg.write16(operand->reg, value);
            break;
        case OP_MEM:
            writeMem16bits(operand->memPtr, value);
            break;
        case OP_IMM:
        case OP_NONE:
        default:
            printf("Unexpected store 16 bits location, operand = %d\n", operand->type);
            break;
    }
}


void Cpu::executeNOP(instruction_t* instr)
{
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeLD8(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);

    /* Add the amount of cycles used. */
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeLD8Inc(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);

    /* Incrementing the HL register. */
    u16 result = reg.read16(RegID_HL) + 1;
    reg.write16(RegID_HL, result);

    /* Add the amount of cycles used. */
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeLD8Dec(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);

    /* Incrementing the HL register. */
    u16 result = reg.read16(RegID_HL) - 1;
    reg.write16(RegID_HL, result);

    /* Add the amount of cycles used. */
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeLD16(instruction_t* instr)
{
    /* Load and store 2 byte of information. */
    u16 value = loadOperand16bits(&(instr->operandSrc));
    storeOperand16bits(&(instr->operandDst), value);

    /* Add the amount of cycles used. */
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeJP(instruction_t* instr)
{
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u8 conditionFlag = instr->extraInfo;

    /* Code execution for: JP (HL) */
    if(conditionFlag == COND_HL)
    {
        reg.setProgramCounter(reg.read16(RegID_HL));
        this->cyclesCompleted += instr->cycleCost;
    }
    else
    {
        /* Get the jump location and check if the condition is true or does not exist. */
        u16 jumpLocation = instr->operandSrc.immediate;
        if(conditionFlag == COND_NONE
        ||(conditionFlag == COND_NZ && zero == false)
        ||(conditionFlag == COND_Z  && zero == true)
        ||(conditionFlag == COND_NC && carry == false)
        ||(conditionFlag == COND_C  && carry == true))
        {
            reg.setProgramCounter(jumpLocation);
            this->cyclesCompleted += instr->cycleCost;
        }
        else
        {
            this->cyclesCompleted += 3;
        }
    }
}


void Cpu::executeJR(instruction_t* instr)
{
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u8 tmp = instr->operandSrc.immediate;
    i8 step = (static_cast<i8> (tmp));
    u8 conditionFlag = instr->extraInfo;

    /* Get the jump location and check if the condition is true or does not exist. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        u16 newPC = reg.getProgramCounter() + step;
        reg.setProgramCounter(newPC);
        this->cyclesCompleted += instr->cycleCost;
    }
    else
    {
        this->cyclesCompleted += 2;
    }
}


/**
 * Bitwise XOR of a value with register A. Then store the result in register A.
 */
void Cpu::executeXOR(instruction_t* instr)
{
    /* Bitwise xor. */
    u8 result = reg.read8(RegID_A) ^ loadOperand8bits(&(instr->operandSrc));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write8(RegID_A, result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeINC8(instruction_t* instr)
{
    u8 original = loadOperand8bits(&(instr->operandDst));
    u8 result = original + 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(halfCarryTest(original, 1));

    storeOperand8bits(&(instr->operandDst), result);
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeDEC8(instruction_t* instr)
{
    u8 original = loadOperand8bits(&(instr->operandDst));
    u8 result = original - 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(true);
    reg.setFlagHalfCarry(halfBorrowTest(original, 1));

    storeOperand8bits(&(instr->operandDst), result);
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeINC16(instruction_t* instr)
{
    u16 result = reg.read16(instr->operandDst.reg) + 1;
    reg.write16(instr->operandDst.reg, result);
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeDEC16(instruction_t* instr)
{
    u16 result = reg.read16(instr->operandDst.reg) - 1;
    reg.write16(instr->operandDst.reg, result);
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeDI(instruction_t* instr)
{
    this->interruptController.disableInterrupts();
    this->cyclesCompleted += instr->cycleCost;
}
































/**
 * This function fetches the next instruction from memory pointed by the program counter. Then it
 * increments the program counter and returns the opcode.
 */
u8 Cpu::fetchNextInstruction()
{
    /* Get the program counter. */
    u16 pc = reg.getProgramCounter();

    /* Fetch the next instruction from memory. */
    u8 opcode = mmu->read(pc);

    /* Increase the program counter by 1 and return the instruction code. */
    reg.incProgramCounter();
    return opcode;
}

/**
 * Fetches 16 bits from memory by stitching 2 bytes together. The first byte is the low byte and
 * the second byte is the high byte.
 */
u16 Cpu::fetchNext16Bits()
{
    /* Get the low and high u8 of the 16 bit address. */
    u16 low = fetchNextInstruction();
    u16 high = fetchNextInstruction();

    /* Construct the address. */
    u16 word = (high << 8) + low;

    return word;
}

/**
 * Read some data from memory specified by an address in a register.
 */
u8 Cpu::readMemFromPointer(RegID memPointer)
{
    /* Get the memory location. */
    u16 memLocation = reg.read16(memPointer);

    /* Get and return the data from memory. */
    return (mmu->read(memLocation));
}

/**
 * TODO: DELETE BECAUSE OUTDATED.
 * Read some data from memory specified by an address in a register.
 */
u8 Cpu::readMem(RegID memPointer)
{
    /* Get the memory location. */
    u16 memLocation = reg.read16(memPointer);

    /* Get and return the data from memory. */
    return (mmu->read(memLocation));
}


/**
 * Read some data from memory specified by an address in a register.
 */
u8 Cpu::readMem8bits(u16 memPointer)
{
    /* Get and return the data from memory. */
    return (mmu->read(memPointer));
}


/**
 * Read some data from memory specified by an address in a register.
 */
u16 Cpu::readMem16bitsFromPointer(RegID memPointer)
{
    u16 memLocation = reg.read16(memPointer);
    u16 word = mmu->read16bits(memLocation);
    return word;
}


u16 Cpu::readMem16bits(u16 memPointer)
{
    u16 word = mmu->read16bits(memPointer);
    return word;
}


/**
 * Writes some data to memory specified by an address in a register.
 */
void Cpu::writeMem(RegID memPointer, u8 data)
{
    /* Get the memory location. */
    u16 memLocation = reg.read16(memPointer);

    /* Get and return the data from memory. */
    mmu->write(memLocation, data);
}


/**
 * Writes some data to memory specified by an address in a register.
 */
void Cpu::writeMem16bits(RegID memPointer, u16 data)
{
    /* Get the memory location. */
    u16 memLocation = reg.read16(memPointer);

    u8 low = (u8) (data & 0xff);
    u8 high = (u8) (data >> 8);

    /* Get and return the data from memory. */
    mmu->write(memLocation, low);
    mmu->write(memLocation + 1, high);
}


void Cpu::checkSignals()
{
    /* Check for an interrupt signal. */
    u16 interruptSignal = this->interruptController.checkForInterrupts();
    if(interruptSignal == 0x0)
        return;
    else
        setupSignalExecution(interruptSignal);
}


/**
 * Pushes the current program counter on the stack and updates the program counter to the signal
 * address.
 */
void Cpu::setupSignalExecution(u16 interruptSignalAddress)
{
    printf("Signal: 0x%x\n", interruptSignalAddress);
    interruptController.disableInterrupts();
    executePUSH(reg.read16(RegID_PC));
    reg.write16(RegID_PC, interruptSignalAddress);
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
void Cpu::executePUSH(u16 val)
{
    /* Get the high and low byte. */
    u8 low = val & 0xff;
    u8 high = (val >> 8) & 0xff;
    u16 sp = reg.getStackPointer();

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
u16 Cpu::executePOP()
{
    /* Get 2 bytes from the stack. */
    u16 val = mmu->read16bits(reg.getStackPointer());

    /* Increases the stack pointer by two. */
    reg.incStackPointer();
    reg.incStackPointer();

    return val;
}


/**
 * Adds a value to the contents of register A and store it in register A.
 */
void Cpu::executeADD8(u8 val)
{
    /* Add the value of register 2 to register 1. */
    u8 result = reg.read8(RegID_A) + val;
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
void Cpu::executeADC(u8 val)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;

    u8 carry;
    if(reg.getFlagCarry())
        carry = 1;
    else
        carry = 0;

    u8 result = reg.read8(RegID_A) + val + carry;

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
void Cpu::executeSUB(u8 val)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;
    u8 result = reg.read8(RegID_A) - val;

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
void Cpu::executeSBC(u8 val)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.read8(RegID_A);
    int val2 = (int) val;

    u8 carry;
    if(reg.getFlagCarry())
        carry = 1;
    else
        carry = 0;

    u8 result = reg.read8(RegID_A) - val - carry;

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
void Cpu::executeAND(u8 val)
{
    /* Bitwise AND. */
    u8 result = reg.read8(RegID_A) & val;

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
void Cpu::executeXOR(u8 val)
{
    /* Bitwise xor. */
    u8 result = reg.read8(RegID_A) ^ val;

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
void Cpu::executeOR(u8 val)
{
    /* Bitwise or. */
    u8 result = reg.read8(RegID_A) | val;

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
void Cpu::executeCP(u8 val)
{
    u8 src = reg.read8(RegID_A);

    reg.setFlagZero(src == val);
    reg.setFlagHalfCarry(src > val);
    reg.setFlagCarry(src < val);
    reg.setFlagSub(true);
}


u8 Cpu::executeINC8(u8 val)
{
    u8 result = val + 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(halfCarryTest(val, 1));

    return result;
}


u8 Cpu::executeDEC8(u8 val)
{
    u8 result = val - 1;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(true);
    reg.setFlagHalfCarry(halfBorrowTest(val, 1));

    /* Store the result. */
    return result;
}


void Cpu::executeINC16(RegID dest)
{
    u16 result = reg.read16(dest) + 1;
    reg.write16(dest, result);
}


void Cpu::executeDEC16(RegID dest)
{
    u16 result = reg.read16(dest) - 1;
    reg.write16(dest, result);
}


void Cpu::executeADD16(RegID dest, int val)
{
    /* Get the values from the registers and do the addition. */
    u16 destVal = reg.read16(dest);
    u16 result = val + destVal;

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
    u16 jumpLocation = fetchNext16Bits();
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
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
    u8 tmp = fetchNextInstruction();
    i8 step = (static_cast<i8> (tmp));

    /* Get the jump location and check if the condition is true or does not exist. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        u16 newPC = reg.getProgramCounter() + step;
        reg.setProgramCounter(newPC);
        cycles = 3;
    }

    return cycles;
}


/**
 * Push the program counter on the stack and adjust the program counter.
 */
int Cpu::executeCALL(u16 jumpLocation, int conditionFlag)
{
    int cycles = 3;
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u16 pc = reg.getProgramCounter();

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
        u16 val = this->executePOP();
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
u8 Cpu::executeSET(u8 src, int bitNumber)
{
    return (src | (1 <<bitNumber));
}


/**
 * Set a specific bit in a byte to 0.
 */
u8 Cpu::executeRES(u8 src, int bitNumber)
{
    return (src & ~(1 << bitNumber));
}


/**
 * Get the bit value of a specific bit in a byte and set the zero flag as the complement of
 * this bit.
 */
void Cpu::executeBIT(u8 src, int bitNumber)
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
u8 Cpu::executeSWAP(u8 src)
{
    u8 result = (src << 4) + (src >> 4);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    return result;
}


/**
 * Shifts the byte to the left. Note: Bit 7 goes to the carry flag and the carry flag to bit 0.
 */
u8 Cpu::executeRL(u8 src)
{
    u8 carry = (u8) reg.getFlagCarry();
    u8 result = (src << 1) | carry;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x80) == 0x80);

    return result;
}


/**
 * Shifts the byte to the right. Note: Bit 0 goes to the carry flag and the carry flag to bit 7.
 */
u8 Cpu::executeRR(u8 src)
{
    u8 carry = ((u8) reg.getFlagCarry()) << 7;
    u8 result = (src >> 1) | carry;

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x1) == 0x1);

    return result;
}


/**
 * Shifts the byte to the left. Note: Bit 7 goes to both the carry flag and bit 0.
 */
u8 Cpu::executeRLC(u8 src)
{
    u8 result = (src << 1) | (src >> 7);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x80) == 0x80);

    return result;
}


/**
 * Shifts the byte to the right. Note: Bit 0 goes to both the carry flag and bit 7.
 */
u8 Cpu::executeRRC(u8 src)
{
    u8 result = (src << 7) | (src >> 1);

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
    u8 src = reg.read8(RegID_A);
    u8 result = executeRL(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite RR with register A and set the zero flag to false.
 */
void Cpu::executeRRA()
{
    u8 src = reg.read8(RegID_A);
    u8 result = executeRR(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite RLC with register A and set the zero flag to false.
 */
void Cpu::executeRLCA()
{
    u8 src = reg.read8(RegID_A);
    u8 result = executeRLC(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite RRC with register A and set the zero flag to false.
 */
void Cpu::executeRRCA()
{
    u8 src = reg.read8(RegID_A);
    u8 result = executeRRC(src);
    reg.write8(RegID_A, result);
    reg.setFlagZero(false);
}


/**
 * Execite SLA on a byte. It shifts the contents 1 bit to the left and bit 7 is stored in the
 * carry flag.
 */
u8 Cpu::executeSLA(u8 src)
{
    bool carry = (src & 0x80) == 0x80;
    u8 result = src << 1;

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
u8 Cpu::executeSRA(u8 src)
{
    bool carry = (src & 0x1) == 0x1;
    u8 result = (src & 0x80) | (src >> 1);

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
u8 Cpu::executeSRL(u8 src)
{
    bool carry = (src & 0x1) == 0x1;
    u8 result = src >> 1;;

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
