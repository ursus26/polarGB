#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include <bitset>
#include "types.h"


const size_t INTERRUPT_VERTICAL_BLANKING = 0x1;
const size_t INTERRUPT_LCDC = 0x2;
const size_t INTERRUPT_TIMER_OVERFLOW = 0x4;
const size_t INTERRUPT_SERIAL_TRANSFER_COMPLETION = 0x8;
const size_t INTERRUPT_JOYPAD = 0x10;

const U16 INTERRUPT_VERTICAL_BLANKING_ADDR = 0x40;
const U16 INTERRUPT_LCDC_ADDR = 0x48;
const U16 INTERRUPT_TIMER_OVERFLOW_ADDR = 0x50;
const U16 INTERRUPT_SERIAL_TRANSFER_COMPLETION_ADDR = 0x58;
const U16 INTERRUPT_JOYPAD_ADDR = 0x60;


class InterruptController
{
public:
    InterruptController();
    ~InterruptController();

    void disableInterrupts();
    void enableInterrupts();

private:
    bool ime;   /* Interrupt Master Enable. 0: disable all interrupts, 1: enable all interrupts. */
    const U16 ieAddr = 0xffff; /* Interrupt Enable register address. */
    const U16 ifAddr = 0xff0f; /* Interrupt Flag register address. */
};


#endif /* INTERRUPT_CONTROLLER_H */
