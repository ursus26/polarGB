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
#include "polarGB/timer.h"


Timer::Timer(std::shared_ptr<InterruptController> interruptController)
{
    assert(interruptController != nullptr);

    this->DIV = 0;
    this->TIMA = 0;
    this->TMA = 0;
    this->TAC = 0;
    this->timaClock = 0;
    this->divClock = 0;
    this->interruptController = interruptController;
}


Timer::~Timer()
{
}


void Timer::update(u8 cycles)
{
    updateDivider(cycles);
    updateTimer(cycles);
}


u8 Timer::read(timerRegister_t reg) const
{
    assert(reg >= RegDIV);
    assert(reg <= RegTAC);

    switch(reg) {
        case RegDIV:
            return this->DIV;
        case RegTIMA:
            return this->TIMA;
        case RegTMA:
            return this->TMA;
        case RegTAC:
            return this->TAC & 0x7;
        default:
            return 0;
    }
}


void Timer::write(timerRegister_t reg, u8 value)
{
    assert(reg >= RegDIV);
    assert(reg <= RegTAC);

    switch(reg)
    {
        case RegDIV:
            this->DIV = 0;
            this->divClock = 0;
            break;
        case RegTIMA:
            this->TIMA = value;
            break;
        case RegTMA:
            this->TMA = value;
            break;
        case RegTAC:
            if((this->TAC & 0x4) == 0 && value & 0x4)
                this->timaClock = 0;
            this->TAC = value & 0x7;
            break;
    }
}


void Timer::updateDivider(u8 cycles)
{
    this->divClock += cycles;
    while(this->divClock >= 64)
    {
        this->divClock -= 64;
        this->DIV++;
    }
}


void Timer::updateTimer(u8 cycles)
{
    if(this->TAC & 0x4)
    {
        /* Update the internal clock. */
        this->timaClock += cycles;

        /* Get the timer frequency. */
        unsigned int clockFreq = 0;
        switch(this->TAC & 0x3)
        {
            case 0:
                clockFreq = 256;
                break;
            case 1:
                clockFreq = 4;
                break;
            case 2:
                clockFreq = 16;
                break;
            case 3:
                clockFreq = 64;
                break;
        }

        /* Update the timer clock. */
        while(this->timaClock >= clockFreq)
        {
            this->timaClock -= clockFreq;
            if(this->TIMA == 255)
            {
                this->interruptController->requestInterrupt(int_timer_overflow);
                this->TIMA = this->TMA;
            }
            else
                this->TIMA++;
        }
    }
}
