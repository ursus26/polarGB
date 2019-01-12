#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "register.h"
#include "mmu.h"


const int CPU_FREQUENCY = 1048575; /* Hz */
const int FPS = 60;
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
    Cpu(Mmu* m);
    ~Cpu();

    void boot();

    /* Basic CPU operations. (Running cycles, fetching and executing instructions) */
    void run(int cycles);
    void runSingleFrame();
    U8 fetchNextInstruction();
    U16 fetchNext16Bits();
    int executeInstruction(U8 opcode);

    /* Read and write to a memory location specified by a register. */
    U8 readMem(RegID memPointer);
    void writeMem(RegID memPointer, U8 data);

private:
    /* Member variables. */
    Register reg;
    Mmu* mmu;
    bool isRunning;
    int cyclesCompleted = 0;


    /* Half-carry and carry test. */
    bool halfCarryTest(int val1, int val2);
    bool halfBorrowTest(int val1, int val2);
    bool carryTest(int val1, int val2);
    bool borrowTest(int val1, int val2);

    /* Execute the extended instruction set. */
    int executeCB();

    /* 8 bit load/store/move instructions */
    void executeLD8(U16 addr, U8 val);

    /* 16 bit load/store/move instructions */
    void executeLD16(RegID dest);
    void executePUSH(U16 val);
    U16 executePOP();

    /* 8 bit arithmetic/logical instructions */
    void executeADD8(U8 val);
    void executeADC(U8 val);
    void executeSUB(U8 val);
    void executeSBC(U8 val);
    void executeAND(U8 val);
    void executeXOR(U8 val);
    void executeOR(U8 val);
    void executeCP(U8 val);
    U8 executeINC8(U8 val);
    U8 executeDEC8(U8 val);

    /* 16 bit arithmetic/logical instructions */
    void executeINC16(RegID dest);
    void executeDEC16(RegID dest);
    void executeADD16(RegID dest, int val);

    /* Jumps/calls */
    int executeJP(int conditionFlag);
    int executeJR(int conditionFlag);
    int executeCALL(U16 jumpLocation, int conditionFlag);
    int executeRET(int conditionFlag);

    /* 	8 bit rotations/shifts and bit instructions */
    U8 executeSET(U8 src, int bitNumber);
    U8 executeRES(U8 src, int bitNumber);
    void executeBIT(U8 src, int bitNumber);
    U8 executeSWAP(U8 src);
    void executeRLA();
    void executeRRA();
    U8 executeRL(U8 src);
    U8 executeRR(U8 src);
    void executeRLCA();
    void executeRRCA();
    U8 executeRLC(U8 src);
    U8 executeRRC(U8 src);
    U8 executeSLA(U8 src);
    U8 executeSRA(U8 src);
    U8 executeSRL(U8 src);
};

#endif /* CPU_H */
