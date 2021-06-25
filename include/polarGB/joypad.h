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

#ifndef JOYPAD_H
#define JOYPAD_H

#include <memory>
#include <SDL2/SDL.h>
#include "types.h"
#include "interrupt_controller.h"


class Joypad
{
public:
    Joypad(std::shared_ptr<InterruptController> interruptController);
    ~Joypad();

    u8 read();
    void write(u8 data);
    void processInput();
    bool getButtonQuit() const;

private:
    std::shared_ptr<InterruptController> interruptController;
    u8 P1;

    /* Buttons pressed. */
    bool buttonQuit;
    bool buttonA;
    bool buttonB;
    bool buttonSelect;
    bool buttonStart;
    bool buttonRight;
    bool buttonLeft;
    bool buttonUp;
    bool buttonDown;

    void processKeyDown(SDL_Keycode keysym);
    void processKeyUp(SDL_Keycode keysym);
};

#endif /* JOYPAD_H */
