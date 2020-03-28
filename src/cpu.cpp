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

#include <stdlib.h>
#include <chrono>
#include <fmt/format.h>
#include "polarGB/cpu.h"
#include "polarGB/log.h"


using namespace std;

Cpu::Cpu()
{
}

Cpu::~Cpu()
{
}


void Cpu::startUp(Mmu* m)
{
    this->mmu = m;
    this->isRunning = true;
    this->cyclesCompleted = 0;
    this->interruptController.startUp(this->mmu);
    this->interruptController.disableInterrupts();

    /* Initialise the registers. */
    this->reg.writeDouble(RegID_AF, 0x01b0);    /* Initialise AF register. */
    this->reg.writeDouble(RegID_BC, 0x0013);    /* Initialise BC register. */
    this->reg.writeDouble(RegID_DE, 0x00d8);    /* Initialise DE register. */
    this->reg.writeDouble(RegID_HL, 0x014d);    /* Initialise HL register. */
    this->reg.writeDouble(RegID_SP, 0xfffe);    /* Initialise the stack. */

    /* Finish the boot sequence by setting the pc counter to 0x100. */
    this->reg.setProgramCounter(0x100);
}


void Cpu::shutDown()
{
    /* Remove the reference to the memory manager but don't delete it since this object did not
     * create the memory manager. */
    this->interruptController.shutDown();
    this->mmu = nullptr;
}


/**
 * Executes a single instruction and returns the amount of CPU cycles used for executing the
 * instruction.
 */
u8 Cpu::step()
{
    /* Check for interrupts before fetching the next instruction. This possibly changes the
     * program counter in order to execute a signal. */
    this->checkSignals();

    /* Fetch the next instruction. */
    Instruction* instr = fetchDecode();
    reg.setProgramCounter(reg.getProgramCounter() + instr->instructionLength);

    printInstructionInfo(instr);

    // if(instr->memoryLocation == 0x282a)
    // {
    //     for(int i = 0; i < 8; i++)
    //     {
    //         for(int j = 0; j < 8; j++)
    //         {
    //             fmt::print("{:x} ", mmu->read(VRAM_START_ADDR + 0x100));
    //         }
    //         fmt::print("\n");
    //     }
    //     exit(0);
    // }

    /* Execute the instruction handler. */
    (this->*(instr->executionFunction))(instr);
    u8 cycleCost = instr->cycleCost;

    /* Instruction clean up. */
    delete instr;

    return cycleCost;
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

    // if(pc == 0x02b6) {
    //     exit(0);
    // }

    return instr;
}


void Cpu::printInstructionInfo(instruction_t *instr)
{
    fmt::print("{:04x}\t{:02x}", instr->memoryLocation, instr->opcode);

    u16 data = 0;
    if(instr->instructionLength == 3)
    {
        if(instr->operandSrc.type == OP_IMM)
            data = instr->operandSrc.immediate;
        else
            data = instr->operandDst.immediate;
        fmt::print(" {:02x} {:02x}    ", data & 0xff, data >> 8);
    }
    else if(instr->instructionLength == 2)
    {
        if(instr->operandSrc.type == OP_IMM)
            data = instr->operandSrc.immediate;
        else
            data = instr->operandDst.immediate;
        fmt::print(" {:02x}         ", data & 0xff);
    }
    else
    {
        /* Padding for allignment. */
        fmt::print("          ");
    }

    fmt::print("\t{}\n", instr->mnemonic);
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


u8 Cpu::loadOperand8bits(Operand* operand)
{
    switch(operand->type)
    {
        case OP_IMM:
            return operand->immediate;
        case OP_IMM_PTR:
            return mmu->read(operand->immediate);
        case OP_REG:
            return reg.readSingle(operand->reg);
        case OP_MEM:
            return mmu->read(reg.readDouble(operand->memPtr));
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
            return reg.readDouble(operand->reg);
        case OP_MEM:
            return mmu->read2Bytes(reg.readDouble(operand->memPtr));
        case OP_NONE:
        case OP_IMM_PTR:
        default:
            return 0;
    }
}


void Cpu::storeOperand8bits(Operand* operand, u8 value)
{
    switch(operand->type)
    {
        case OP_REG:
            reg.writeSingle(operand->reg, value);
            break;
        case OP_MEM:
            mmu->write(reg.readDouble(operand->memPtr), value);
            break;
        case OP_IMM_PTR:
            mmu->write(operand->immediate, value);
            break;
        default:
            fmt::print("Unexpected store 8 bits location, operand = {}\n", operand->type);
            break;
    }
}


void Cpu::storeOperand16bits(Operand* operand, u16 value)
{
    switch(operand->type)
    {
        case OP_REG:
            reg.writeDouble(operand->reg, value);
            break;
        case OP_MEM:
            mmu->write2Bytes(reg.readDouble(operand->memPtr), value);
            break;
        case OP_IMM_PTR:
            mmu->write2Bytes(operand->immediate, value);
            break;
        default:
            fmt::print("Unexpected store 16 bits location, operand = %d\n", operand->type);
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
    u16 result = reg.readDouble(RegID_HL) + 1;
    reg.writeDouble(RegID_HL, result);

    /* Add the amount of cycles used. */
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeLD8Dec(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);

    /* Incrementing the HL register. */
    u16 result = reg.readDouble(RegID_HL) - 1;
    reg.writeDouble(RegID_HL, result);

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
        reg.setProgramCounter(reg.readDouble(RegID_HL));
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


void Cpu::executeCALL(instruction_t* instr)
{
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u16 callLocation = instr->operandSrc.immediate;
    u8 conditionFlag = instr->extraInfo;

    /* Execute the call instruction by pushing the PC to the stack and setting a new PC. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        _executePUSH(reg.getProgramCounter());

        reg.setProgramCounter(callLocation);
        this->cyclesCompleted += instr->cycleCost;
    }
    else
    {
        instr->cycleCost = 3;
        this->cyclesCompleted += 2;
    }
}


void Cpu::executeRET(instruction_t* instr)
{
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u8 conditionFlag = instr->extraInfo;

    /* Write the program counter to the stack if the condition matches. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_IE)
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        instr->operandDst.type = OP_REG;
        instr->operandDst.reg = RegID_PC;

        this->executePOP(instr);
        // reg.setProgramCounter(val);

        // /* Adjust the stack pointer. */
        // reg.incStackPointer();
        // reg.incStackPointer();

        if(conditionFlag != COND_NONE && conditionFlag != COND_IE)
            instr->cycleCost = 5;

        if(conditionFlag == COND_IE)
            interruptController.enableInterrupts(false);
    }
    else
    {
        instr->cycleCost = 2;
    }



}


/**
 * Adds a value to the contents of register A and store it in register A.
 */
void Cpu::executeADD8(instruction_t* instr)
{
    /* Add the value of the source to the destination. */
    u8 result = loadOperand8bits(&(instr->operandSrc)) + loadOperand8bits(&(instr->operandDst));
    int val1 = (int) loadOperand8bits(&(instr->operandSrc));
    int val2 = (int) loadOperand8bits(&(instr->operandDst));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);                       /* Set flag if the result equals 0. */
    reg.setFlagSub(false);                              /* Always reset the Sub flag. */
    reg.setFlagHalfCarry(halfCarryTest(val1, val2));
    reg.setFlagCarry(carryTest(val1, val2));

    /* Store the addition. */
    storeOperand8bits(&(instr->operandDst), result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Adds a value and the carry bit to the contents of register A and stores it in register A.
 */
void Cpu::executeADC(instruction_t* instr)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) loadOperand8bits(&(instr->operandSrc));
    int val2 = (int) loadOperand8bits(&(instr->operandDst));

    u8 carry;
    if(reg.getFlagCarry())
        carry = 1;
    else
        carry = 0;

    u8 result = loadOperand8bits(&(instr->operandSrc)) + loadOperand8bits(&(instr->operandDst));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(false);               /* Always reset the Sub flag. */
    reg.setFlagHalfCarry(halfCarryTest(val1, val2 + carry));
    reg.setFlagCarry(carryTest(val1, val2 + carry));

    /* Store the addition. */
    storeOperand8bits(&(instr->operandDst), result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Subtracts a value from register A.
 */
void Cpu::executeSUB(instruction_t* instr)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) reg.readSingle(RegID_A);
    int val2 = (int) loadOperand8bits(&(instr->operandSrc));
    u8 result = reg.readSingle(RegID_A) - loadOperand8bits(&(instr->operandSrc));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true);               /* Always set the Sub flag. */
    reg.setFlagHalfCarry(halfBorrowTest(val1, val2));
    reg.setFlagCarry(borrowTest(val1, val2));

    /* Store the addition. */
    reg.writeSingle(RegID_A, result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Subtracts a value and the carry bit from register A.
 */
void Cpu::executeSBC(instruction_t* instr)
{
    /* Add the value of register 2 to register 1. */
    int val1 = (int) loadOperand8bits(&(instr->operandSrc));
    int val2 = (int) loadOperand8bits(&(instr->operandDst));

    u8 carry;
    if(reg.getFlagCarry())
        carry = 1;
    else
        carry = 0;

    u8 result = loadOperand8bits(&(instr->operandSrc)) - loadOperand8bits(&(instr->operandDst)) - carry;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true);               /* Always set the Sub flag. */
    reg.setFlagHalfCarry(halfBorrowTest(val1, val2 - carry));
    reg.setFlagCarry(borrowTest(val1, val2 - carry));

    /* Store the addition. */
    storeOperand8bits(&(instr->operandDst), result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Bitwise AND of a value with register A. Then store the result in register A.
 */
void Cpu::executeAND(instruction_t* instr)
{
    /* Bitwise AND. */
    u8 result = loadOperand8bits(&(instr->operandSrc)) & reg.readSingle(RegID_A);

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(true);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.writeSingle(RegID_A, result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Bitwise XOR of a value with register A. Then store the result in register A.
 */
void Cpu::executeXOR(instruction_t* instr)
{
    /* Bitwise xor. */
    u8 result = reg.readSingle(RegID_A) ^ loadOperand8bits(&(instr->operandSrc));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.writeSingle(RegID_A, result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Bitwise OR of a value with register A. Then store the result in register A.
 */
void Cpu::executeOR(instruction_t* instr)
{
    /* Bitwise or. */
    u8 result = reg.readSingle(RegID_A) | loadOperand8bits(&(instr->operandSrc));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.writeSingle(RegID_A, result);

    /* Clock cycles */
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Execute the CP instruction and set the flags based on the result.
 */
void Cpu::executeCP(instruction_t* instr)
{
    /* Input values. */
    u8 val = loadOperand8bits(&(instr->operandSrc));
    u8 registerA = reg.readSingle(RegID_A);

    reg.setFlagZero(registerA == val);
    reg.setFlagHalfCarry(registerA > val);
    reg.setFlagCarry(registerA < val);
    reg.setFlagSub(true);

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
    u16 result = reg.readDouble(instr->operandDst.reg) + 1;
    reg.writeDouble(instr->operandDst.reg, result);
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeDEC16(instruction_t* instr)
{
    u16 result = reg.readDouble(instr->operandDst.reg) - 1;
    reg.writeDouble(instr->operandDst.reg, result);
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeDI(instruction_t* instr)
{
    this->interruptController.disableInterrupts();
    this->cyclesCompleted += instr->cycleCost;
}


void Cpu::executeEI(instruction_t* instr)
{
    this->interruptController.enableInterrupts(true);
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Entry point for push instruction.
 */
void Cpu::executePUSH(instruction_t* instr)
{
    _executePUSH(reg.readDouble(instr->operandSrc.reg));
    this->cyclesCompleted += instr->cycleCost;
}


/**
 * Pushes the contents of a register pair to the stack. The stack grows down.
 */
void Cpu::_executePUSH(u16 val)
{
    /* Fetch stack pointer and check for stack overflow. */
    u16 sp = reg.getStackPointer();
    // if(sp - 2 < HRAM_START_ADDR)
    // {
        // fmt::print("SP: {:#x}\n", sp);
    //     Log::printError("Error, stack overflow!");
    //     this->isRunning = false;
    //     exit(1);
    //     return;
    // }

    /* Write the value to the stack. */
    mmu->write2Bytes(sp - 2, val);

    /* Decrease the stack pointer by two. */
    reg.decStackPointer();
    reg.decStackPointer();
}


/**
 * Pops 2 bytes from the stack. The stack grows down.
 */
void Cpu::executePOP(instruction_t* instr)
{
    /* Get 2 bytes from the stack and check if we can pop at least 2 bytes from the stack. */
    u16 sp = reg.getStackPointer();
    // if(sp >= 0xfffe - 1)
    // {
    //     Log::printError("Error, stack underflow!");
    //     this->isRunning = false;
    //     return;
    // }

    /* Pop value from stack. */
    u16 val = mmu->read2Bytes(sp);

    /* Increases the stack pointer by two. */
    reg.incStackPointer();
    reg.incStackPointer();

    /* Store result in register. */
    reg.writeDouble(instr->operandDst.reg, val);
    this->cyclesCompleted += instr->cycleCost;
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
    interruptController.disableInterrupts();
    _executePUSH(reg.readDouble(RegID_PC));
    reg.writeDouble(RegID_PC, interruptSignalAddress);
}


// void Cpu::executeADD16(RegID dest, int val)
// {
//     /* Get the values from the registers and do the addition. */
//     u16 destVal = reg.read16(dest);
//     u16 result = val + destVal;
//
//     /* Set the flags. */
//     reg.setFlagSub(false);
//     if(val < 0)
//     {
//         reg.setFlagHalfCarry(halfBorrowTest(destVal, val));
//         reg.setFlagCarry(borrowTest(destVal, val));
//     }
//     else
//     {
//         reg.setFlagHalfCarry(halfCarryTest(destVal, val));
//         reg.setFlagCarry(carryTest(destVal, val));
//     }
//     if(dest == RegID_SP)
//         reg.setFlagZero(false);
//
//     /* Store the result. */
//     reg.writeDouble(dest, result);
// }
//
//
// /**
//  * Set a specific bit in a byte to 1.
//  */
// u8 Cpu::executeSET(u8 src, int bitNumber)
// {
//     return (src | (1 <<bitNumber));
// }
//
//
// /**
//  * Set a specific bit in a byte to 0.
//  */
// u8 Cpu::executeRES(u8 src, int bitNumber)
// {
//     return (src & ~(1 << bitNumber));
// }
//
//
// /**
//  * Get the bit value of a specific bit in a byte and set the zero flag as the complement of
//  * this bit.
//  */
// void Cpu::executeBIT(u8 src, int bitNumber)
// {
//     bool bitValue = (src >> bitNumber) & 0x1;
//
//     reg.setFlagZero(!bitValue);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(true);
// }
//
//
// /**
//  * Swaps the first 4 bits with the last 4 bits in a byte. For example 0xAF will result in 0xFA and
//  * 0x10 will result in 0x1.
//  */
// u8 Cpu::executeSWAP(u8 src)
// {
//     u8 result = (src << 4) + (src >> 4);
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry(false);
//
//     return result;
// }
//
//
// /**
//  * Shifts the byte to the left. Note: Bit 7 goes to the carry flag and the carry flag to bit 0.
//  */
// u8 Cpu::executeRL(u8 src)
// {
//     u8 carry = (u8) reg.getFlagCarry();
//     u8 result = (src << 1) | carry;
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry((src & 0x80) == 0x80);
//
//     return result;
// }
//
//
// /**
//  * Shifts the byte to the right. Note: Bit 0 goes to the carry flag and the carry flag to bit 7.
//  */
// u8 Cpu::executeRR(u8 src)
// {
//     u8 carry = ((u8) reg.getFlagCarry()) << 7;
//     u8 result = (src >> 1) | carry;
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry((src & 0x1) == 0x1);
//
//     return result;
// }
//
//
// /**
//  * Shifts the byte to the left. Note: Bit 7 goes to both the carry flag and bit 0.
//  */
// u8 Cpu::executeRLC(u8 src)
// {
//     u8 result = (src << 1) | (src >> 7);
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry((src & 0x80) == 0x80);
//
//     return result;
// }
//
//
// /**
//  * Shifts the byte to the right. Note: Bit 0 goes to both the carry flag and bit 7.
//  */
// u8 Cpu::executeRRC(u8 src)
// {
//     u8 result = (src << 7) | (src >> 1);
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry((src & 0x1) == 0x1);
//
//     return result;
// }
//
//
// /**
//  * Execute RL with register A and set the zero flag to false.
//  */
// void Cpu::executeRLA()
// {
//     u8 src = reg.read8(RegID_A);
//     u8 result = executeRL(src);
//     reg.write8(RegID_A, result);
//     reg.setFlagZero(false);
// }
//
//
// /**
//  * Execite RR with register A and set the zero flag to false.
//  */
// void Cpu::executeRRA()
// {
//     u8 src = reg.read8(RegID_A);
//     u8 result = executeRR(src);
//     reg.write8(RegID_A, result);
//     reg.setFlagZero(false);
// }
//
//
// /**
//  * Execite RLC with register A and set the zero flag to false.
//  */
// void Cpu::executeRLCA()
// {
//     u8 src = reg.read8(RegID_A);
//     u8 result = executeRLC(src);
//     reg.write8(RegID_A, result);
//     reg.setFlagZero(false);
// }
//
//
// /**
//  * Execite RRC with register A and set the zero flag to false.
//  */
// void Cpu::executeRRCA()
// {
//     u8 src = reg.read8(RegID_A);
//     u8 result = executeRRC(src);
//     reg.write8(RegID_A, result);
//     reg.setFlagZero(false);
// }
//
//
// /**
//  * Execite SLA on a byte. It shifts the contents 1 bit to the left and bit 7 is stored in the
//  * carry flag.
//  */
// u8 Cpu::executeSLA(u8 src)
// {
//     bool carry = (src & 0x80) == 0x80;
//     u8 result = src << 1;
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry(carry);
//
//     return result;
// }
//
//
// /**
//  * Execite SRA on a byte. It shifts the contents 1 bit to the right. Bit 7 goes to itself and
//  * bit 6 and bit 0 goes to the carry flag.
//  */
// u8 Cpu::executeSRA(u8 src)
// {
//     bool carry = (src & 0x1) == 0x1;
//     u8 result = (src & 0x80) | (src >> 1);
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry(carry);
//
//     return result;
// }
//
//
// /**
//  * Execite SRA on a byte. It shifts the contents 1 bit to the right. Bit 7 goes to itself and
//  * bit 6 and bit 0 goes to the carry flag.
//  */
// u8 Cpu::executeSRL(u8 src)
// {
//     bool carry = (src & 0x1) == 0x1;
//     u8 result = src >> 1;;
//
//     reg.setFlagZero(result == 0);
//     reg.setFlagSub(false);
//     reg.setFlagHalfCarry(false);
//     reg.setFlagCarry(carry);
//
//     return result;
// }
