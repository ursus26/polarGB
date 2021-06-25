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


const u8 P10_MASK = 0x01;
const u8 P11_MASK = 0x02;
const u8 P12_MASK = 0x04;
const u8 P13_MASK = 0x08;
const u8 P14_MASK = 0x10;
const u8 P15_MASK = 0x20;


Joypad::Joypad(std::shared_ptr<InterruptController> interruptController)
{
    assert(interruptController != nullptr);

    this->interruptController = interruptController;
    this->P1 = 0x3f;
    this->buttonQuit = false;
    this->buttonA = false;
    this->buttonB = false;
    this->buttonSelect = false;
    this->buttonStart = false;
    this->buttonRight = false;
    this->buttonLeft = false;
    this->buttonUp = false;
    this->buttonDown = false;
}


Joypad::~Joypad()
{
}


void Joypad::processInput()
{
    /* Check if we should close our window. */
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0)
    {
        switch (event.type) {
            case SDL_QUIT:
                this->buttonQuit = true;
                break;
            case SDL_KEYDOWN:
                processKeyDown(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                processKeyUp(event.key.keysym.sym);
                break;
            default:
                break;
        }
    }
}


u8 Joypad::read()
{
    this->P1 |= 0xf;
    if((this->P1 & P14_MASK) == 0)
    {
        if(this->buttonRight)
            this->P1 &= ~P10_MASK;
        if(this->buttonLeft)
            this->P1 &= ~P11_MASK;
        if(this->buttonUp)
            this->P1 &= ~P12_MASK;
        if(this->buttonDown)
            this->P1 &= ~P13_MASK;
    }
    else if((this->P1 & P15_MASK) == 0)
    {
        if(this->buttonA)
            this->P1 &= ~P10_MASK;
        if(this->buttonB)
            this->P1 &= ~P11_MASK;
        if(this->buttonSelect)
            this->P1 &= ~P12_MASK;
        if(this->buttonStart)
            this->P1 &= ~P13_MASK;
    }
    else
    {
        this->P1 = 0x3f;
    }

    return this->P1;
}


void Joypad::write(u8 data)
{
    this->P1 = data & 0x30;
}


bool Joypad::getButtonQuit() const
{
    return this->buttonQuit;
}


void Joypad::processKeyDown(SDL_Keycode keysym)
{
    switch(keysym) {
        case SDLK_ESCAPE:
            this->buttonQuit = true;
            break;
        case SDLK_w:
            this->buttonUp = true;
            break;
        case SDLK_a:
            this->buttonLeft = true;
            break;
        case SDLK_s:
            this->buttonDown = true;
            break;
        case SDLK_d:
            this->buttonRight = true;
            break;
        case SDLK_k:
            this->buttonA = true;
            break;
        case SDLK_l:
            this->buttonB = true;
            break;
        case SDLK_o:
            this->buttonSelect = true;
            break;
        case SDLK_p:
            this->buttonStart = true;
            break;
        default:
            break;
    }
}


void Joypad::processKeyUp(SDL_Keycode keysym)
{
    switch(keysym) {
        case SDLK_ESCAPE:
            this->buttonQuit = false;
            break;
        case SDLK_w:
            this->buttonUp = false;
            break;
        case SDLK_a:
            this->buttonLeft = false;
            break;
        case SDLK_s:
            this->buttonDown = false;
            break;
        case SDLK_d:
            this->buttonRight = false;
            break;
        case SDLK_k:
            this->buttonA = false;
            break;
        case SDLK_l:
            this->buttonB = false;
            break;
        case SDLK_o:
            this->buttonSelect = false;
            break;
        case SDLK_p:
            this->buttonStart = false;
            break;
        default:
            break;
    }
}
