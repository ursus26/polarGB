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
#include <assert.h>
#include <fmt/format.h>
#include "polarGB/cpu.h"
#include "polarGB/log.h"


using namespace std;

Cpu::Cpu()
{
    this->mmu = nullptr;
    this->interruptController = nullptr;
    this->isRunning = false;
    this->currentInstruction = nullptr;
    this->timer = nullptr;
}

Cpu::~Cpu()
{
}


void Cpu::startUp(Mmu* m, InterruptController* interruptController, Timer* timer)
{
    assert(m != nullptr);
    assert(interruptController != nullptr);
    assert(timer != nullptr);

    this->mmu = m;
    this->interruptController = interruptController;
    this->isRunning = true;
    this->interruptController->disableInterrupts();
    this->currentInstruction = new instruction_t;
    this->timer = timer;

    /* Initialise the registers. */
    this->reg.write(RegID_AF, 0x01b0);    /* Initialise AF register. */
    this->reg.write(RegID_BC, 0x0013);    /* Initialise BC register. */
    this->reg.write(RegID_DE, 0x00d8);    /* Initialise DE register. */
    this->reg.write(RegID_HL, 0x014d);    /* Initialise HL register. */
    this->reg.write(RegID_SP, 0xfffe);    /* Initialise the stack. */
    this->reg.write(RegID_PC, 0x100);     /* Initialise the program counter */
}


void Cpu::shutDown()
{
    delete this->currentInstruction;
    this->currentInstruction = nullptr;

    this->interruptController = nullptr;
    this->timer = nullptr;
    this->mmu = nullptr;
}


/**
 * Executes a single instruction and returns the amount of CPU cycles used for executing the
 * instruction.
 */
u8 Cpu::step()
{
    /* Fetch the next instruction. */
    Instruction* instr = fetchDecode();
    reg.write(RegID_PC, reg.read(RegID_PC) + instr->instructionLength);

    // if(instr->memoryLocation == 0x028b)
    // {
    //     fmt::print("Restarting game\n");
    // }
    // printInstructionInfo(instr);

    /* Execute the instruction handler. */
    (this->*(instr->executionFunction))(instr);
    u8 cycleCost = instr->cycleCost;

    /* Check for interrupts before fetching the next instruction. This possibly changes the
     * program counter in order to execute a signal. */
    this->checkSignals();

    return cycleCost;
}


Cpu::instruction_t * Cpu::fetchDecode()
{
    /* Get the program counter. */
    u16 pc = reg.read(RegID_PC);

    /* Fetch the next instruction from memory. */
    u8 opcode = mmu->read(pc);

    currentInstruction->memoryLocation = pc;
    currentInstruction->opcode = opcode;
    decodeOpcode(currentInstruction, opcode);

    return currentInstruction;
}


void Cpu::checkSignals()
{
    /* Check for an interrupt signal. */
    u16 interruptSignal = this->interruptController->checkForInterrupts();
    if(interruptSignal == 0x0)
        return;
    else
        setupSignalExecution(interruptSignal);
}


/**
 * Pushes the current program counter on the stack and updates the program counter to the signal
 * address.
 */
void Cpu::setupSignalExecution(u8 interruptSignal)
{
    interruptController->disableInterrupts();
    interruptController->resetInterruptFlag(interruptSignal);
    u16 interruptVector = interruptController->getInterruptVector(interruptSignal);

    _executePUSH(reg.read(RegID_PC));
    reg.write(RegID_PC, interruptVector);
}


void Cpu::printInstructionInfo(instruction_t *instr)
{
    fmt::print("{:04x}\t{:02x}", instr->memoryLocation, instr->opcode);

    /* Read the next 2 bytes after the opcode and print them if needed. */
    u16 data = mmu->read2Bytes(instr->memoryLocation + 1);
    if(instr->instructionLength == 3)
    {
        fmt::print(" {:02x} {:02x}    ", data & 0xff, data >> 8);
    }
    else if(instr->instructionLength == 2)
    {
        fmt::print(" {:02x}         ", data & 0xff);
    }
    else
    {
        /* Padding for allignment. */
        fmt::print("          ");
    }

    fmt::print("\t{}\n", instr->mnemonic);
}


u8 Cpu::loadOperand8bits(operand_t* operand)
{
    switch(operand->type)
    {
        case OP_IMM:
            return operand->immediate;
        case OP_IMM_PTR:
            return mmu->read(operand->immediate);
        case OP_REG:
            return reg.read(operand->reg);
        case OP_MEM:
            return mmu->read(reg.read(operand->memPtr));
        case OP_NONE:
        default:
            return 0;
    }
}


u16 Cpu::loadOperand16bits(operand_t* operand)
{
    assert(operand->type != OP_NONE);
    assert(operand->type != OP_IMM_PTR);

    switch(operand->type)
    {
        case OP_IMM:
            return operand->immediate;
        case OP_REG:
            return reg.read(operand->reg);
        case OP_MEM:
            return mmu->read2Bytes(reg.read(operand->memPtr));
        default:
            return 0;
    }
}


void Cpu::storeOperand8bits(operand_t* operand, u8 value)
{
    assert(operand->type != OP_NONE);
    assert(operand->type != OP_IMM);

    switch(operand->type)
    {
        case OP_REG:
            reg.write(operand->reg, value);
            break;
        case OP_MEM:
            mmu->write(reg.read(operand->memPtr), value);
            break;
        case OP_IMM_PTR:
            mmu->write(operand->immediate, value);
            break;
        default:
            fmt::print("Unexpected store 8 bits location, operand = {}\n", operand->type);
            break;
    }
}


void Cpu::storeOperand16bits(operand_t* operand, u16 value)
{
    switch(operand->type)
    {
        case OP_REG:
            reg.write(operand->reg, value);
            break;
        case OP_MEM:
            mmu->write2Bytes(reg.read(operand->memPtr), value);
            break;
        case OP_IMM_PTR:
            mmu->write2Bytes(operand->immediate, value);
            break;
        default:
            fmt::print("Unexpected store 16 bits location, operand = %d\n", operand->type);
            break;
    }
}


void Cpu::executeNOP(instruction_t*)
{
}


void Cpu::executeLD8(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);
}


void Cpu::executeLD8Inc(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);

    /* Incrementing the HL register. */
    u16 result = reg.read(RegID_HL) + 1;
    reg.write(RegID_HL, result);
}


void Cpu::executeLD8Dec(instruction_t* instr)
{
    /* Load and store 1 byte of information. */
    u8 value = loadOperand8bits(&(instr->operandSrc));
    storeOperand8bits(&(instr->operandDst), value);

    /* Incrementing the HL register. */
    u16 result = reg.read(RegID_HL) - 1;
    reg.write(RegID_HL, result);
}


void Cpu::executeLD16(instruction_t* instr)
{
    /* Load and store 2 byte of information. */
    u16 value = loadOperand16bits(&(instr->operandSrc));
    storeOperand16bits(&(instr->operandDst), value);
}


void Cpu::executeLDHL(instruction_t* instr)
{
    i8 src = (i8) loadOperand8bits(&instr->operandSrc);
    u16 sp = reg.read(RegID_SP);
    u16 result = sp + src;
    storeOperand16bits(&instr->operandDst, result);

    /* Set or reset the flags. */
    reg.setFlagCarry((result & 0xff) < (sp & 0xff));
    reg.setFlagHalfCarry((result & 0xf) < (sp & 0xf));
    reg.setFlagZero(false);
    reg.setFlagSub(false);
}


void Cpu::executeJP(instruction_t* instr)
{
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u8 conditionFlag = instr->extraInfo;

    /* Code execution for: JP (HL) */
    if(conditionFlag == COND_HL)
    {
        reg.write(RegID_PC, reg.read(RegID_HL));
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
            reg.write(RegID_PC, jumpLocation);
        }
        else
        {
            instr->cycleCost = 3;
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
        reg.write(RegID_PC, reg.read(RegID_PC) + step);
    }
    else
    {
        instr->cycleCost = 2;
    }
}


void Cpu::executeCALL(instruction_t* instr)
{
    bool zero = reg.getFlagZero();
    bool carry = reg.getFlagCarry();
    u8 conditionFlag = instr->extraInfo;

    /* Execute the call instruction by pushing the PC to the stack and setting a new PC. */
    if(conditionFlag == COND_NONE
    ||(conditionFlag == COND_NZ && zero == false)
    ||(conditionFlag == COND_Z  && zero == true)
    ||(conditionFlag == COND_NC && carry == false)
    ||(conditionFlag == COND_C  && carry == true))
    {
        /* Push the current PC to the stack. */
        _executePUSH(reg.read(RegID_PC));

        /* Set the new program counter. */
        u16 callLocation = loadOperand16bits(&instr->operandSrc);
        reg.write(RegID_PC, callLocation);
    }
    else
    {
        instr->cycleCost = 3;
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
        /* Pop the old program counter from the stack. */
        instr->operandSrc.type = OP_MEM;
        instr->operandSrc.memPtr = RegID_SP;
        instr->operandDst.type = OP_REG;
        instr->operandDst.reg = RegID_PC;
        this->executePOP(instr);

        /* Re-enable the IME flag if the instruction originated from an interrupt. */
        if(conditionFlag == COND_IE)
            interruptController->enableInterrupts(false);
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
    u16 param1 = (u16)loadOperand8bits(&(instr->operandSrc));
    u16 param2 = (u16)loadOperand8bits(&(instr->operandDst));
    u8 result = (u8) (0xff & (param1 + param2));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);                       /* Set flag if the result equals 0. */
    reg.setFlagSub(false);                              /* Always reset the Sub flag. */;
    reg.setFlagCarry(param1 + param2 > 0xff);
    reg.setFlagHalfCarry((param1 & 0xf) + (param2 & 0xf) > 0xf);

    /* Store the addition. */
    storeOperand8bits(&(instr->operandDst), result);
}


/**
 * Adds a value and the carry bit to the contents of register A and stores it in register A.
 */
void Cpu::executeADC(instruction_t* instr)
{
    u16 param1 = (u16)loadOperand8bits(&(instr->operandSrc));
    u16 param2 = (u16)loadOperand8bits(&(instr->operandDst));
    u8 carry = 0;
    if(reg.getFlagCarry())
        carry = 1;
    u8 result = (u8) (0xff & (param1 + param2 + carry));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);                       /* Set flag if the result equals 0. */
    reg.setFlagSub(false);                              /* Always reset the Sub flag. */;
    reg.setFlagCarry(param1 + param2 + carry > 0xff);
    reg.setFlagHalfCarry((param1 & 0xf) + (param2 & 0xf) + carry > 0xf);

    /* Store the addition. */
    storeOperand8bits(&(instr->operandDst), result);
}


/**
 * Subtracts a value from register A.
 */
void Cpu::executeSUB(instruction_t* instr)
{
    u8 param1 = reg.read(RegID_A);
    u8 param2 = loadOperand8bits(&(instr->operandSrc));
    u8 result = param1 - param2;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true);               /* Always set the Sub flag. */
    reg.setFlagHalfCarry((param2 & 0xf) > (param1 & 0xf));
    reg.setFlagCarry(param2 > param1);

    /* Store the addition. */
    reg.write(RegID_A, result);
}


/**
 * Subtracts a value and the carry bit from register A.
 */
void Cpu::executeSBC(instruction_t* instr)
{
    u8 param1 = reg.read(RegID_A);
    u8 param2 = loadOperand8bits(&(instr->operandSrc));
    u8 carry = 0;
    if(reg.getFlagCarry())
        carry = 1;
    u8 result = param1 - param2 - carry;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true); /* Always set the Sub flag. */
    reg.setFlagHalfCarry((param2 & 0xf) + carry > (param1 & 0xf));
    reg.setFlagCarry((u16)param2 + (u16)carry > (u16)param1);

    /* Store the addition. */
    reg.write(RegID_A, result);
}


/**
 * Bitwise AND of a value with register A. Then store the result in register A.
 */
void Cpu::executeAND(instruction_t* instr)
{
    /* Bitwise AND. */
    u8 result = loadOperand8bits(&(instr->operandSrc)) & reg.read(RegID_A);

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(true);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write(RegID_A, result);
}


/**
 * Bitwise XOR of a value with register A. Then store the result in register A.
 */
void Cpu::executeXOR(instruction_t* instr)
{
    /* Bitwise xor. */
    u8 result = reg.read(RegID_A) ^ loadOperand8bits(&(instr->operandSrc));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write(RegID_A, result);
}


/**
 * Bitwise OR of a value with register A. Then store the result in register A.
 */
void Cpu::executeOR(instruction_t* instr)
{
    /* Bitwise or. */
    u8 result = reg.read(RegID_A) | loadOperand8bits(&(instr->operandSrc));

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    /* Store the result. */
    reg.write(RegID_A, result);
}


/**
 * Execute the CP instruction and set the flags based on the result.
 */
void Cpu::executeCP(instruction_t* instr)
{
    /* Input values. */
    u8 val = loadOperand8bits(&(instr->operandSrc));
    u8 registerA = reg.read(RegID_A);

    reg.setFlagZero(registerA == val);
    reg.setFlagSub(true);

    reg.setFlagHalfCarry((registerA & 0xf) < (val & 0xf));
    reg.setFlagCarry(registerA < val);
}


void Cpu::executeINC8(instruction_t* instr)
{
    u16 param = (u16)loadOperand8bits(&(instr->operandDst));
    u8 result = (u8) (0xff & (param + 1));

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry((param & 0xf) == 0xf);

    storeOperand8bits(&(instr->operandDst), result);
}


void Cpu::executeDEC8(instruction_t* instr)
{
    u8 param = loadOperand8bits(&(instr->operandDst));
    u8 result = param - 1;

    /* Set or reset the flags. */
    reg.setFlagZero(result == 0); /* Set flag if the result equals 0. */
    reg.setFlagSub(true);               /* Always set the Sub flag. */
    reg.setFlagHalfCarry((param & 0xf) == 0);

    storeOperand8bits(&(instr->operandDst), result);
}


void Cpu::executeINC16(instruction_t* instr)
{
    reg.write(instr->operandDst.reg, reg.read(instr->operandDst.reg) + 1);
}


void Cpu::executeDEC16(instruction_t* instr)
{
    reg.write(instr->operandDst.reg, reg.read(instr->operandDst.reg) - 1);
}


void Cpu::executeADD16(instruction_t* instr)
{
    u16 src1 = loadOperand16bits(&instr->operandSrc);
    u16 src2 = loadOperand16bits(&instr->operandDst);
    storeOperand16bits(&instr->operandDst, src1 + src2);

    u32 result = src1 + src2;
    reg.setFlagSub(false);
    reg.setFlagCarry(result > 0xffff);
    reg.setFlagHalfCarry((src2 & 0xfff) + (src1 & 0xfff) > 0xfff);
}


void Cpu::executeADD16SPe(instruction_t* instr)
{
    i8 src = (i8) loadOperand8bits(&instr->operandSrc);
    u16 sp = reg.read(RegID_SP);
    u16 result = sp + src;
    storeOperand16bits(&instr->operandDst, result);

    /* Set or reset the flags. */
    reg.setFlagCarry((result & 0xff) < (sp & 0xff));
    reg.setFlagHalfCarry((result & 0xf) < (sp & 0xf));
    reg.setFlagZero(false);
    reg.setFlagSub(false);
}


void Cpu::executeDI(instruction_t*)
{
    this->interruptController->disableInterrupts();
}


void Cpu::executeEI(instruction_t*)
{
    this->interruptController->enableInterrupts(true);
}


void Cpu::executeCPL(instruction_t*)
{
    u8 value = ~reg.read(RegID_A);
    reg.write(RegID_A, value);

    reg.setFlagSub(true);
    reg.setFlagHalfCarry(true);
}


/**
 * CCF: Complement Carry Flag.
 */
void Cpu::executeCCF(instruction_t*)
{
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(!reg.getFlagCarry());
}


/**
 * SCF: Set Carry Flag.
 */
void Cpu::executeSCF(instruction_t*)
{
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(true);
}


/**
 * DAA: Decimal Adjust Accumulator.
 */
void Cpu::executeDAA(instruction_t*)
{
    u8 src = reg.read(RegID_A);
    bool halfCarryFlag = reg.getFlagHalfCarry();
    bool carryFlag = reg.getFlagCarry();
    bool subFlag = reg.getFlagSub();
    u8 result = src;

    if(subFlag == false)
    {
        if(halfCarryFlag || (src & 0xf) > 9)
            result += 0x6;
        if(carryFlag || (result & 0xf0) > 0x9f)
        {
            result += 0x6;
            reg.setFlagCarry(true);
        }
        else
            reg.setFlagCarry(false);
    }
    else
    {
        if(carryFlag)
            result -= 0x60;

        if(halfCarryFlag)
            result -= 0x6;
    }

    reg.write(RegID_A, result);
    reg.setFlagZero(result == 0);
    reg.setFlagHalfCarry(false);
}


/**
 * Entry point for push instruction.
 */
void Cpu::executePUSH(instruction_t* instr)
{
    u16 val = loadOperand16bits(&instr->operandSrc);
    _executePUSH(val);
}


/**
 * Pushes the input argument to the stack. The stack grows down.
 */
void Cpu::_executePUSH(u16 val)
{
    /* Fetch stack pointer and check for stack overflow. */
    u16 sp = reg.read(RegID_SP);

    /* Write the value to the stack. */
    mmu->write2Bytes(sp - 2, val);

    /* Decrease the stack pointer by two. */
    reg.write(RegID_SP, sp - 2);
}


/**
 * Pops 2 bytes from the stack. The stack grows down.
 */
void Cpu::executePOP(instruction_t* instr)
{
    /* Pop value from stack. */
    u16 val = loadOperand16bits(&instr->operandSrc);

    /* Increases the stack pointer by two. */
    reg.write(RegID_SP, reg.read(RegID_SP) + 2);

    /* Store result in register. */
    storeOperand16bits(&instr->operandDst, val);
}


/**
 * Swaps the first 4 bits with the last 4 bits in a byte. For example 0xAF will result in 0xFA and
 * 0x10 will result in 0x1.
 */
void Cpu::executeSWAP(instruction_t* instr)
{
    u8 val = loadOperand8bits(&instr->operandSrc);

    u8 result = (val << 4) + (val >> 4);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(false);

    storeOperand8bits(&instr->operandDst, result);
}


/**
 * Get the bit value of a specific bit in a byte and set the zero flag as the complement of
 * this bit.
 */
void Cpu::executeBIT(instruction_t* instr)
{
    u8 bitNumber = instr->extraInfo;
    u8 src = loadOperand8bits(&instr->operandSrc);
    bool bitValue = (src >> bitNumber) & 0x1;

    reg.setFlagZero(!bitValue);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(true);
}


/**
 * Set a specific bit in a byte to 1. The bit number is stored in the extraInfo field.
 */
void Cpu::executeSET(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    u8 result = src | (1 << instr->extraInfo);
    storeOperand8bits(&instr->operandDst, result);
}


/**
 * Set a specific bit of a byte to 0.
 */
void Cpu::executeRES(instruction_t* instr)
{
    u8 bitNumber = instr->extraInfo;
    u8 srcVal = loadOperand8bits(&instr->operandSrc);
    u8 dstVal = (srcVal & ~(1 << bitNumber));
    storeOperand8bits(&instr->operandDst, dstVal);
}


/**
 * Execute RLC with register A and set the zero flag to false.
 */
void Cpu::executeRLCA(instruction_t* instr)
{
    this->executeRLC(instr);
    reg.setFlagZero(false);
}


/**
 * Execute RL with register A and set the zero flag to false.
 */
void Cpu::executeRLA(instruction_t* instr)
{
    this->executeRL(instr);
    reg.setFlagZero(false);
}


/**
 * Execute RRCA with register A and set the zero flag to false.
 */
void Cpu::executeRRCA(instruction_t* instr)
{
    this->executeRRC(instr);
    reg.setFlagZero(false);
}


/**
 * Execute RR with register A and set the zero flag to false.
 */
void Cpu::executeRRA(instruction_t* instr)
{
    this->executeRR(instr);
    reg.setFlagZero(false);
}


/**
 * Shifts the byte to the left. Note: Bit 7 goes to both the carry flag and bit 0.
 */
void Cpu::executeRLC(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    u8 result = (src << 1) | (src >> 7);
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x80) == 0x80);
}


/**
 * Shifts the byte to the left. Note: Bit 7 goes to the carry flag and the carry flag to bit 0.
 */
void Cpu::executeRL(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    u8 carry = (u8) reg.getFlagCarry();
    u8 result = (src << 1) | carry;
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x80) == 0x80);
}


/**
 * Shifts the byte to the right. Note: Bit 0 goes to both the carry flag and bit 7.
 */
void Cpu::executeRRC(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    u8 result = (src << 7) | (src >> 1);
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x1) == 0x1);
}


/**
 * Shifts the byte to the right. Note: Bit 0 goes to the carry flag and the carry flag to bit 7.
 */
void Cpu::executeRR(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    u8 carry = ((u8) reg.getFlagCarry()) << 7;
    u8 result = (src >> 1) | carry;
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry((src & 0x1) == 0x1);
}


/**
 * Execute SLA on a byte. It shifts the contents 1 bit to the left and bit 7 is stored in the
 * carry flag.
 */
void Cpu::executeSLA(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    bool carry = (src & 0x80) == 0x80;
    u8 result = src << 1;
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(carry);
}


/**
 * Execute SRA on a byte. It shifts the contents 1 bit to the right. Bit 7 goes to itself and
 * bit 6 and bit 0 goes to the carry flag.
 */
void Cpu::executeSRA(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    bool carry = (src & 0x1) == 0x1;
    u8 result = (src & 0x80) | (src >> 1);
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(carry);
}


/**
 * Execute SRA on a byte. It shifts the contents 1 bit to the right. Bit 7 goes to itself and
 * bit 6 and bit 0 goes to the carry flag.
 */
void Cpu::executeSRL(instruction_t* instr)
{
    u8 src = loadOperand8bits(&instr->operandSrc);
    bool carry = (src & 0x1) == 0x1;
    u8 result = src >> 1;
    storeOperand8bits(&instr->operandDst, result);

    reg.setFlagZero(result == 0);
    reg.setFlagSub(false);
    reg.setFlagHalfCarry(false);
    reg.setFlagCarry(carry);
}
