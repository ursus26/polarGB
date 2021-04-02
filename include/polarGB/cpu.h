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

#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "register.h"
#include "mmu.h"
#include "interrupt_controller.h"


// const int FPS = 60;
// const double FRAME_TIME = 1.0 / (double) FPS;
// const int CPU_FREQUENCY = 1048575; /* Hz */
// const int MAX_INSTRUCTIONS_PER_FRAME = CPU_FREQUENCY / FPS;


enum ConditionFlag
{
    COND_NONE = 0,
    COND_HL = 1,
    COND_NZ = 2,
    COND_Z = 3,
    COND_NC = 4,
    COND_C = 5,
    COND_IE = 6 /* Used for RETI instruction. */
};


enum OperandType
{
    OP_NONE = 0,
    OP_IMM,
    OP_IMM_PTR,
    OP_REG,
    OP_MEM
};


typedef struct Operand
{
    u8 type;
    u16 immediate;
    RegID reg;
    RegID memPtr;
} operand_t;


class Cpu
{
public:
    typedef struct Instruction
    {
        u16 memoryLocation;                             /* Location of the instruction */
        u8 opcode;                                      /* Opcode on the memory location */
        u8 instructionLength;                           /* Length of the instruction */
        char mnemonic[16];                              /* Mnemonic of the instruction, used for debugging */
        operand_t operandSrc;                           /* Source operand of the instruction */
        operand_t operandDst;                           /* Destination operand of the instruction */
        u8 cycleCost;                                   /* Cost of the instruction in clock cycles */
        u8 extraInfo;                                   /* Additional info used for some execution function */
        void (Cpu::*executionFunction)(Instruction *);  /* Funtion that can execute this instruction. */
    } instruction_t;

    Cpu();
    ~Cpu();

    void startUp(Mmu* m, InterruptController* interruptController);
    void shutDown();

    u8 step();

private:
    /* Member variables. */
    Register reg;
    Mmu* mmu;
    InterruptController* interruptController;
    bool isRunning;
    instruction_t* currentInstruction;

    /* Signal handling. */
    void checkSignals();
    void setupSignalExecution(u8 interruptSignal);

    /* Instruction decoding.  */
    instruction_t* fetchDecode();
    void decodeOpcode(instruction_t *instr, u8 opcode);
    void decodePrefixedOpcode(instruction_t *instr);
    void printInstructionInfo(instruction_t *instr);

    /* Instruction helper functions. */
    u8 loadOperand8bits(operand_t* instr);
    u16 loadOperand16bits(operand_t* instr);
    void storeOperand8bits(operand_t* instr, u8 value);
    void storeOperand16bits(operand_t* instr, u16 value);

    /**
     * Instruction functions
     */

    /* 8-bit load instructions */
    void executeLD8(instruction_t* instr);
    void executeLD8Inc(instruction_t* instr);
    void executeLD8Dec(instruction_t* instr);

    /* 16-bit load instructions */
    void executeLD16(instruction_t* instr);
    void executeLDHL(instruction_t* instr);

    /* Jump/call instructions */
    void executeJP(instruction_t* instr);
    void executeJR(instruction_t* instr);
    void executeCALL(instruction_t* instr);
    void executeRET(instruction_t* instr);

    /* 8-bit arithmetic/logical instructions */
    void executeADD8(instruction_t* instr);
    void executeADC(instruction_t* instr);
    void executeSUB(instruction_t* instr);
    void executeSBC(instruction_t* instr);
    void executeAND(instruction_t* instr);
    void executeXOR(instruction_t* instr);
    void executeOR(instruction_t* instr);
    void executeCP(instruction_t* instr);
    void executeINC8(instruction_t* instr);
    void executeDEC8(instruction_t* instr);

    /* 16-bit arithmetic/logical instructions */
    void executeINC16(instruction_t* instr);
    void executeDEC16(instruction_t* instr);
    void executeADD16(instruction_t* instr);
    void executeADD16SPe(instruction_t* instr);

    /* Misc/control instructions */
    void executeNOP(instruction_t* instr);
    void executeDI(instruction_t* instr);
    void executeEI(instruction_t* instr);
    void executeCPL(instruction_t* instr);
    void executeCCF(instruction_t* instr);
    void executeSCF(instruction_t* instr);
    void executeDAA(instruction_t* instr);

    /* 8-bit shift, rotate and bit instructions */
    void executeSWAP(instruction_t* instr);
    void executeBIT(instruction_t* instr);
    void executeSET(instruction_t* instr);
    void executeRES(instruction_t* instr);
    void executeRLCA(instruction_t* instr);
    void executeRLA(instruction_t* instr);
    void executeRRCA(instruction_t* instr);
    void executeRRA(instruction_t* instr);
    void executeRLC(instruction_t* instr);
    void executeRL(instruction_t* instr);
    void executeRRC(instruction_t* instr);
    void executeRR(instruction_t* instr);
    void executeSLA(instruction_t* instr);
    void executeSRA(instruction_t* instr);
    void executeSRL(instruction_t* instr);

    /* Stack functions */
    void executePUSH(instruction_t* instr);
    void _executePUSH(u16 value);   /* Convenient function for the push instr and starting interrupts. */
    void executePOP(instruction_t* instr);
};

#endif /* CPU_H */
