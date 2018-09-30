#include "bank_manager.h"
#include <iostream>

using namespace std;


BankManager::BankManager(unsigned int nBanks, unsigned int bankSize, U16 startAddr)
{
    this->nBanks = nBanks;
    this->bankSize = bankSize;
    this->activeBank = 1;
    this->startAddr = startAddr;
    this->mem = new U8[nBanks * bankSize];
}


BankManager::~BankManager()
{
    delete[] this->mem;
}


void BankManager::switchBank(int newActiveBank)
{
    if(newActiveBank == 0 || (unsigned int) newActiveBank >= nBanks)
    {
        cerr << "Error, cannot switch to memory bank " << newActiveBank << endl;
        return;
    }

    this->activeBank = newActiveBank;
}


U8 BankManager::fetch(U16 addr)
{
    U16 normalizedAddr = addr - startAddr;

    if(normalizedAddr < this->bankSize)
        return this->mem[normalizedAddr];
    else
        return this->mem[normalizedAddr + (this->activeBank - 1) * this->bankSize];
}


void BankManager::write(U16 addr, U8 data)
{
    U16 normalizedAddr = addr - startAddr;

    if(normalizedAddr < this->bankSize)
        this->mem[normalizedAddr] = data;
    else
        this->mem[normalizedAddr + (this->activeBank - 1) * this->bankSize] = data;
}
