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

#include <assert.h>
#include <fmt/format.h>
#include "polarGB/interrupt_controller.h"


InterruptController::InterruptController()
{
}


InterruptController::~InterruptController()
{
}


void InterruptController::startUp(Mmu* mmuInstanceCopy)
{
    this->ime = false;
    this->delayed_enable = false;
    this->mmu = mmuInstanceCopy;
}


void InterruptController::shutDown()
{
    this->mmu = nullptr;
}


/**
 * Disable the Interrupt Master Enable (IME) flag when called by the DI instruction or when an
 * interrupt is being executed.
 */
void InterruptController::disableInterrupts()
{
    this->ime = false;
}


/**
 * Enables the Interrupt Master Enable (IME) flag. It is enabled 1 instruction later if it is
 * called by the EI instruction but instantly when called by the RETI instruction.
 */
void InterruptController::enableInterrupts(bool delayed_enable)
{
    if(!delayed_enable)
        this->ime = true;
    else
        this->delayed_enable = true;
}


/**
 * Resets the interrupt flag in the IF register. The type of flag is specified by the function arg.
 * Possible values are: INTERRUPT_VERTICAL_BLANKING, INTERRUPT_LCDC, INTERRUPT_TIMER_OVERFLOW,
 * INTERRUPT_SERIAL_TRANSFER_COMPLETION, and INTERRUPT_JOYPAD.
 */
void InterruptController::resetInterruptFlag(u8 interruptFlag)
{
    assert(interruptFlag <= INTERRUPT_JOYPAD);

    u8 interruptFlags = this->mmu->read(IF_ADDR) & ~interruptFlag;
    this->mmu->write(IF_ADDR, interruptFlags);
}


/**
 * Check if an interrupt needs to be executed by looking at the Interrupt Master Enable (IME) flag.
 * If enabled and an interrupt has to be processed then return a signal greather than 0 that
 * represents the type of interrupt. There are 5 different interrupt signals:
 * - INTERRUPT_VERTICAL_BLANKING          = 0x01
 * - INTERRUPT_LCDC                       = 0x02
 * - INTERRUPT_TIMER_OVERFLOW             = 0x04
 * - INTERRUPT_SERIAL_TRANSFER_COMPLETION = 0x08
 * - INTERRUPT_JOYPAD                     = 0x10
 *
 * In case no interrupt has to be processed we return 0.
 */
u8 InterruptController::checkForInterrupts()
{
    /* Enable the IME for the next instruction after EI instruction is encountered. */
    if(this->delayed_enable)
    {
        this->ime = true;
        this->delayed_enable = false;
        return 0x0;
    }

    /* Stop if IME is not enabled. */
    if(!this->ime)
        return 0x0;

    /* Read the flags from IF and IE registers. */
    this->processInterruptRegister();

    /* Check the 5 interrupts that need to processed in order of priority. */
    if(this->verticalBlankEnabled && this->verticalBlankRequested)
        return INTERRUPT_VERTICAL_BLANKING;
    else if(this->LCDCEnabled && this->LCDCRequested)
        return INTERRUPT_LCDC;
    else if(this->timerOverflowEnabled && this->timerOverflowRequested)
        return INTERRUPT_TIMER_OVERFLOW;
    else if(this->serialEnabled && this->serialRequested)
        return INTERRUPT_SERIAL_TRANSFER_COMPLETION;
    else if(this->joypadEnabled && this->joypadRequested)
        return INTERRUPT_JOYPAD;
    else
        return 0x0;
}


void InterruptController::processInterruptRegister()
{
    u8 interruptFlags = this->mmu->read(IF_ADDR);
    u8 interruptEnable = this->mmu->read(IE_ADDR);

    this->verticalBlankRequested = (interruptFlags & INTERRUPT_VERTICAL_BLANKING) == INTERRUPT_VERTICAL_BLANKING;
    this->verticalBlankEnabled = (interruptEnable & INTERRUPT_VERTICAL_BLANKING) == INTERRUPT_VERTICAL_BLANKING;
    this->LCDCRequested = (interruptFlags & INTERRUPT_LCDC) == INTERRUPT_LCDC;
    this->LCDCEnabled = (interruptEnable & INTERRUPT_LCDC) == INTERRUPT_LCDC;
    this->timerOverflowRequested = (interruptFlags & INTERRUPT_TIMER_OVERFLOW) == INTERRUPT_TIMER_OVERFLOW;
    this->timerOverflowEnabled = (interruptEnable & INTERRUPT_TIMER_OVERFLOW) == INTERRUPT_TIMER_OVERFLOW;
    this->serialRequested = (interruptFlags & INTERRUPT_SERIAL_TRANSFER_COMPLETION) == INTERRUPT_SERIAL_TRANSFER_COMPLETION;
    this->serialEnabled = (interruptEnable & INTERRUPT_SERIAL_TRANSFER_COMPLETION) == INTERRUPT_SERIAL_TRANSFER_COMPLETION;
    this->joypadRequested = (interruptFlags & INTERRUPT_JOYPAD) == INTERRUPT_JOYPAD;
    this->joypadEnabled = (interruptEnable & INTERRUPT_JOYPAD) == INTERRUPT_JOYPAD;
}


u16 InterruptController::getInterruptVector(u8 signal)
{
    assert(signal <= INTERRUPT_JOYPAD);
    assert(signal != 0);

    switch(signal)
    {
        case INTERRUPT_VERTICAL_BLANKING:
            return INTERRUPT_VERTICAL_BLANKING_ADDR;
        case INTERRUPT_LCDC:
            return INTERRUPT_LCDC;
        case INTERRUPT_TIMER_OVERFLOW:
            return INTERRUPT_TIMER_OVERFLOW_ADDR;
        case INTERRUPT_SERIAL_TRANSFER_COMPLETION:
            return INTERRUPT_SERIAL_TRANSFER_COMPLETION_ADDR;
        case INTERRUPT_JOYPAD:
            return INTERRUPT_JOYPAD_ADDR;
        default:
            fmt::print(stderr, "InterruptController::getInterruptVector | Signal: {:#x} is an invalid signal.\n", signal);
            fmt::print(stderr, "InterruptController::getInterruptVector | Probably multiple signals are given at the same time\n");
            exit(EXIT_FAILURE);
    }
}
