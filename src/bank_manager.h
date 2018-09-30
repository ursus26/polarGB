#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include "types.h"

class BankManager
{
public:
    BankManager(unsigned int nBanks, unsigned int bankSize, U16 startAddr);
    ~BankManager();

    /* Switch out banks. */
    void switchBank(int newActiveBank);

    /* Fetch and write data from memory. */
    U8 fetch(U16 addr);
    void write(U16 addr, U8 data);

private:
    unsigned int nBanks;
    unsigned int bankSize;
    unsigned int activeBank;
    U16 startAddr;
    U8* mem;
};

#endif /* BANK_MANAGER_H */
