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

#ifndef TIMER_H
#define TIMER_H

#include "types.h"
#include "interrupt_controller.h"


typedef enum TimerRegister
{
    RegDIV = 0xff04,
    RegTIMA = 0xff05,
    RegTMA = 0xff06,
    RegTAC = 0xff07,
} timerRegister_t;


class Timer
{
public:
    Timer(InterruptController* interruptController);
    ~Timer();

    void update(u8 cycles);
    u8 read(timerRegister_t reg) const;
    void write(timerRegister_t reg, u8 value);

private:
    /* Registers */
    u8 DIV;     /* Address: 0xff04 */
    u8 TIMA;    /* Address: 0xff05 */
    u8 TMA;     /* Address: 0xff06 */
    u8 TAC;     /* Address: 0xff07 */
    unsigned int timaClock;
    unsigned int divClock;

    InterruptController* interruptController;

    void updateDivider(u8 cycles);
    void updateTimer(u8 cycles);
};

#endif /* EMULATOR_H */
