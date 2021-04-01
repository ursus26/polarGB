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

#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include <bitset>
#include "types.h"


typedef enum InterruptSignal
{
    vertical_blanking = 0x1,
    lcdc = 0x2,
    timer_overflow = 0x4,
    serial_transfer_completion = 0x8,
    joypad = 0x10
} interrupt_t;



const u8 INTERRUPT_VERTICAL_BLANKING            = 0x1;
const u8 INTERRUPT_LCDC                         = 0x2;
const u8 INTERRUPT_TIMER_OVERFLOW               = 0x4;
const u8 INTERRUPT_SERIAL_TRANSFER_COMPLETION   = 0x8;
const u8 INTERRUPT_JOYPAD                       = 0x10;

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

    void disableInterrupts();
    void enableInterrupts(bool delayed_enable);

    u8 checkForInterrupts();
    void resetInterruptFlag(u8 interruptFlag);
    u16 getInterruptVector(u8 interruptSignal);

    void requestInterrupt(interrupt_t interruptSignal);

    u8 getIF() const;
    u8 getIE() const;
    void setIF(u8 value);
    void setIE(u8 value);

private:
    /* Interrupt registers. */
    u8 IF; /* Address: 0xff0f*/
    u8 IE; /* Address: 0xffff*/
    bool IME; /* Interrupt Master Enable. 0: disable all interrupts, 1: enable all interrupts. */
    bool delayed_enable;
};


#endif /* INTERRUPT_CONTROLLER_H */
