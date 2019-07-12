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
#include "video.h"
#include "interrupt_controller.h"


const int CPU_FREQUENCY = 1048575; /* Hz */
const int FPS = 60;
const double FRAME_TIME = 1.0 / (double) FPS;
const int MAX_INSTRUCTIONS_PER_FRAME = CPU_FREQUENCY / FPS;

enum ConditionFlag
{
    COND_HL = -2,
    COND_NONE = -1,
    COND_NZ = 0,
    COND_Z = 1,
    COND_NC = 2,
    COND_C = 3
};


class Cpu
{
public:
    Cpu(Mmu* m, Video* vid);
    ~Cpu();

    void boot();

    /* Basic CPU operations. (Running cycles, fetching and executing instructions) */
    void run();
    void runNCycles(int cycles);
    void runSingleFrame();
    u8 fetchNextInstruction();
    u16 fetchNext16Bits();
    int executeInstruction(u8 opcode);

    /* Read and write to a memory location specified by a register. */
    u8 readMem(RegID memPointer);
    void writeMem(RegID memPointer, u8 data);

    void executeADD8(u8 val);

private:
    /* Member variables. */
    Register reg;
    Mmu* mmu;
    Video* video;
    InterruptController interruptController;
    bool isRunning;
    int cyclesCompleted = 0;

    /* Signal handling. */
    void checkSignals();
    void setupSignalExecution(u16 interruptSignal);

    /* Half-carry and carry test. */
    bool halfCarryTest(int val1, int val2);
    bool halfBorrowTest(int val1, int val2);
    bool carryTest(int val1, int val2);
    bool borrowTest(int val1, int val2);

    /* Execute the extended instruction set. */
    int executeCB();

    /* 8 bit load/store/move instructions */
    void executeLD8(u16 addr, u8 val);

    /* 16 bit load/store/move instructions */
    void executeLD16(RegID dest);
    void executePUSH(u16 val);
    u16 executePOP();

    /* 8 bit arithmetic/logical instructions */
    // void executeADD8(u8 val);
    void executeADC(u8 val);
    void executeSUB(u8 val);
    void executeSBC(u8 val);
    void executeAND(u8 val);
    void executeXOR(u8 val);
    void executeOR(u8 val);
    void executeCP(u8 val);
    u8 executeINC8(u8 val);
    u8 executeDEC8(u8 val);

    /* 16 bit arithmetic/logical instructions */
    void executeINC16(RegID dest);
    void executeDEC16(RegID dest);
    void executeADD16(RegID dest, int val);

    /* Jumps/calls */
    int executeJP(int conditionFlag);
    int executeJR(int conditionFlag);
    int executeCALL(u16 jumpLocation, int conditionFlag);
    int executeRET(int conditionFlag);

    /* 	8 bit rotations/shifts and bit instructions */
    u8 executeSET(u8 src, int bitNumber);
    u8 executeRES(u8 src, int bitNumber);
    void executeBIT(u8 src, int bitNumber);
    u8 executeSWAP(u8 src);
    void executeRLA();
    void executeRRA();
    u8 executeRL(u8 src);
    u8 executeRR(u8 src);
    void executeRLCA();
    void executeRRCA();
    u8 executeRLC(u8 src);
    u8 executeRRC(u8 src);
    u8 executeSLA(u8 src);
    u8 executeSRA(u8 src);
    u8 executeSRL(u8 src);
};

#endif /* CPU_H */
