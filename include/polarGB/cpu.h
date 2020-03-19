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
    COND_C = 5
};


enum OperandType
{
    OP_NONE = 0,
    OP_IMM = 1,
    OP_REG = 2,
    OP_MEM = 4
};


typedef struct Operand
{
    u8 type;
    u16 immediate;
    RegID reg;
    RegID memPtr;
} Operand;


class Cpu
{
public:
    typedef struct Instruction
    {
        u16 memoryLocation;                             /* Location of the instruction */
        u8 opcode;                                      /* Opcode on the memory location */
        u8 instructionLength;                           /* Length of the instruction */
        char mnemonic[16];                              /* Mnemonic of the instruction, used for debugging */
        Operand operandSrc;                             /* Source operand of the instruction */
        Operand operandDst;                             /* Destination operand of the instruction */
        u8 cycleCost;                                   /* Cost of the instruction in clock cycles */
        u8 extraInfo;                                   /* Additional info used for some execution function */
        void (Cpu::*executionFunction)(Instruction *);  /* Funtion that can execute this instruction. */
    } instruction_t;

    Cpu();
    ~Cpu();

    void startUp(Mmu* m);
    void shutDown();

    u8 step();

private:
    /* Member variables. */
    Register reg;
    Mmu* mmu;
    InterruptController interruptController;
    bool isRunning;
    int cyclesCompleted = 0;

    /* Signal handling. */
    void checkSignals();
    void setupSignalExecution(u16 interruptSignal);

    /* old functions TODO: delete  */
    u8 fetchNextInstruction();
    u16 fetchNext16Bits();
    int executeInstruction(u8 opcode);

    instruction_t* fetchDecode();
    void decodeOpcode(instruction_t *instr, u8 opcode);
    void printInstructionInfo(instruction_t *instr);

    /* Read and write to a memory location specified by a register. */
    u8 readMemFromPointer(RegID memPointer);
    u8 readMem8bits(u16 memPointer);
    u16 readMem16bitsFromPointer(RegID memPointer);
    u16 readMem16bits(u16 memPointer);
    void writeMem(RegID memPointer, u8 data);
    void writeMem16bits(RegID memPointer, u16 data);

    /* Half-carry and carry test. */
    bool halfCarryTest(int val1, int val2);
    bool halfBorrowTest(int val1, int val2);
    bool carryTest(int val1, int val2);
    bool borrowTest(int val1, int val2);

    /* Instruction helper functions. */
    u8 loadOperand8bits(Operand* instr);
    u16 loadOperand16bits(Operand* instr);
    void storeOperand8bits(Operand* instr, u8 value);
    void storeOperand16bits(Operand* instr, u16 value);


    /**
     * Instruction functions
     */

    /* 8-bit load instructions */
    void executeLD8(instruction_t* instr);
    void executeLD8Inc(instruction_t* instr);
    void executeLD8Dec(instruction_t* instr);
    void executeLD8InternalRam(instruction_t* instr);

    /* 16-bit load instructions */
    void executeLD16(instruction_t* instr);

    /* Jump/call instructions */
    void executeJP(instruction_t* instr);
    void executeJR(instruction_t* instr);

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

    /* Misc/control instructions */
    void executeNOP(instruction_t* instr);
    void executeDI(instruction_t* instr);
    void executeEI(instruction_t* instr);

    /* 8-bit shift, rotate and bit instructions */


    /* Stack functions */
    void executePUSH(instruction_t* instr);
    void _executePUSH(u16 value);   /* Convenient function for the push instr and starting interrupts. */
    void executePOP(instruction_t* instr);



    /**
     * OUTDATED instruction functions
     */

    // /* Execute the extended instruction set. */
    // int executeCB();
    //
    // /* 16 bit load/store/move instructions */
    // void executeLD16(RegID dest);
    // void executePUSH(u16 val);
    // u16 executePOP();
    //
    // /* 8 bit arithmetic/logical instructions */
    // void executeADD8(u8 val);
    // void executeADC(u8 val);
    // void executeSUB(u8 val);
    // void executeSBC(u8 val);
    // void executeAND(u8 val);
    // void executeXOR(u8 val);
    // void executeOR(u8 val);
    // void executeCP(u8 val);
    // u8 executeINC8(u8 val);
    // u8 executeDEC8(u8 val);
    //
    // /* 16 bit arithmetic/logical instructions */
    // void executeINC16(RegID dest);
    // void executeDEC16(RegID dest);
    // void executeADD16(RegID dest, int val);
    //
    // /* Jumps/calls */
    // int executeJP(int conditionFlag);
    // int executeJR(int conditionFlag);
    // int executeCALL(u16 jumpLocation, int conditionFlag);
    // int executeRET(int conditionFlag);
    //
    // /* 	8 bit rotations/shifts and bit instructions */
    // u8 executeSET(u8 src, int bitNumber);
    // u8 executeRES(u8 src, int bitNumber);
    // void executeBIT(u8 src, int bitNumber);
    // u8 executeSWAP(u8 src);
    // void executeRLA();
    // void executeRRA();
    // u8 executeRL(u8 src);
    // u8 executeRR(u8 src);
    // void executeRLCA();
    // void executeRRCA();
    // u8 executeRLC(u8 src);
    // u8 executeRRC(u8 src);
    // u8 executeSLA(u8 src);
    // u8 executeSRA(u8 src);
    // u8 executeSRL(u8 src);
};

#endif /* CPU_H */
