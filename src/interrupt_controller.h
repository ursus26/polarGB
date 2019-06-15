#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include <bitset>
#include "types.h"
#include "mmu.h"


const size_t INTERRUPT_VERTICAL_BLANKING            = 0x1;
const size_t INTERRUPT_LCDC                         = 0x2;
const size_t INTERRUPT_TIMER_OVERFLOW               = 0x4;
const size_t INTERRUPT_SERIAL_TRANSFER_COMPLETION   = 0x8;
const size_t INTERRUPT_JOYPAD                       = 0x10;

const u16 INTERRUPT_VERTICAL_BLANKING_ADDR          = 0x40;
const u16 INTERRUPT_LCDC_ADDR                       = 0x48;
const u16 INTERRUPT_TIMER_OVERFLOW_ADDR             = 0x50;
const u16 INTERRUPT_SERIAL_TRANSFER_COMPLETION_ADDR = 0x58;
const u16 INTERRUPT_JOYPAD_ADDR                     = 0x60;


class InterruptController
{
public:
    InterruptController();
    ~InterruptController();

    void initialise(Mmu* mmuInstanceCopy);

    void disableInterrupts();
    void enableInterrupts(bool delayed_enable);

    u16 checkForInterrupts();

private:
    Mmu* mmu;
    bool ime;   /* Interrupt Master Enable. 0: disable all interrupts, 1: enable all interrupts. */
    bool delayed_enable;
    const u16 interruptEnableRegister = 0xffff; /* Interrupt Enable register address. */
    const u16 interruptFlagsRegister = 0xff0f;  /* Interrupt Flag register address. */

    bool verticalBlankRequested;
    bool verticalBlankEnabled;

    bool LCDCRequested;
    bool LCDCEnabled;

    bool timerOverflowRequested;
    bool timerOverflowEnabled;

    bool serialRequested;
    bool serialEnabled;

    bool joypadRequested;
    bool joypadEnabled;

    void processInterruptRegister();
    void handleInterrupt(size_t interruptJumpAddress);
};


#endif /* INTERRUPT_CONTROLLER_H */
