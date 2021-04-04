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
    this->IF = 0;
    this->IE = 0;
    this->IME = false;
    this->delayed_enable = false;
}


InterruptController::~InterruptController()
{
}


/**
 * Disable the Interrupt Master Enable (IME) flag when called by the DI instruction or when an
 * interrupt is being executed.
 */
void InterruptController::disableInterrupts()
{
    this->IME = false;
}


/**
 * Enables the Interrupt Master Enable (IME) flag. It is enabled 1 instruction later if it is
 * called by the EI instruction but instantly when called by the RETI instruction.
 */
void InterruptController::enableInterrupts(bool delayed_enable)
{
    if(!delayed_enable)
        this->IME = true;
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

    u8 interruptFlags = this->IF & ~interruptFlag;
    this->IF = interruptFlags;
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
 * If the cpu is halted then we ignore the IME.
 */
u8 InterruptController::checkInterrupts(bool cpuHalted)
{
    u8 returnCode = 0;

    if(this->IME || cpuHalted)
    {
        /* Check for interrupts in order of priority. */
        for(int i = 0; i < 5; i++)
        {
            bool flag = (this->IF >> i) & 0x1;
            bool enabled = (this->IE >> i) & 0x1;

            if(flag && enabled)
            {
                switch(i)
                {
                    case 0: returnCode = INTERRUPT_VERTICAL_BLANKING; break;
                    case 1: returnCode = INTERRUPT_LCDC; break;
                    case 2: returnCode = INTERRUPT_TIMER_OVERFLOW; break;
                    case 3: returnCode = INTERRUPT_SERIAL_TRANSFER_COMPLETION; break;
                    case 4: returnCode = INTERRUPT_JOYPAD; break;
                }
            }
        }
    }

    /* Enable the IME for the next instruction after EI instruction is encountered. */
    if(this->delayed_enable)
    {
        this->IME = true;
        this->delayed_enable = false;
    }

    return returnCode;
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


void InterruptController::requestInterrupt(interrupt_t interruptSignal)
{
    this->IF |= interruptSignal;
}


u8 InterruptController::getIF() const
{
    return this->IF & 0x1f;
}


u8 InterruptController::getIE() const
{
    return this->IE & 0x1f;
}


void InterruptController::setIF(u8 value)
{
    this->IF = value & 0x1f;
}


void InterruptController::setIE(u8 value)
{
    this->IE = value & 0x1f;
}
