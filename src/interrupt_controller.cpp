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

#include "polarGB/interrupt_controller.h"


InterruptController::InterruptController()
{
    this->ime = false;
    this->delayed_enable = false;
}


InterruptController::~InterruptController()
{
    this->mmu = nullptr;
}


void InterruptController::initialise(Mmu* mmuInstanceCopy)
{
    this->mmu = mmuInstanceCopy;
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
 * Check if an interrupt needs to be executed by looking at the Interrupt Master Enable (IME) flag.
 * If enabled and an interrupt has to be processed then return a signal greather than 0 that
 * represents the start of the interrupt code. There are 5 signals with their instruction address:
 * - INTERRUPT_VERTICAL_BLANKING_ADDR          = 0x40
 * - INTERRUPT_LCDC_ADDR                       = 0x48
 * - INTERRUPT_TIMER_OVERFLOW_ADDR             = 0x50
 * - INTERRUPT_SERIAL_TRANSFER_COMPLETION_ADDR = 0x58
 * - INTERRUPT_JOYPAD_ADDR                     = 0x60
 *
 * In case no interrupt has to be processed we return 0.
 */
u16 InterruptController::checkForInterrupts()
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
        return INTERRUPT_VERTICAL_BLANKING_ADDR;
    else if(this->LCDCEnabled && this->LCDCRequested)
        return INTERRUPT_LCDC_ADDR;
    else if(this->timerOverflowEnabled && this->timerOverflowRequested)
        return INTERRUPT_TIMER_OVERFLOW_ADDR;
    else if(this->serialEnabled && this->serialRequested)
        return INTERRUPT_SERIAL_TRANSFER_COMPLETION_ADDR;
    else if(this->joypadEnabled && this->joypadRequested)
        return INTERRUPT_JOYPAD_ADDR;
    else
        return 0x0;
}


void InterruptController::processInterruptRegister()
{
    u8 interruptFlags = this->mmu->read(this->interruptFlagsRegister);
    u8 interruptEnable = this->mmu->read(this->interruptEnableRegister);

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
