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
#include "polarGB/joypad.h"


Joypad::Joypad(std::shared_ptr<InterruptController> interruptController)
{
    assert(interruptController != nullptr);

    this->interruptController = interruptController;
    this->P1 = 0x0;
}


Joypad::~Joypad()
{
}


u8 Joypad::read() const
{
    return this->P1;
}


void Joypad::write(u8 data)
{
    this->P1 = data & 0x30;
}
