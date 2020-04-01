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

#include <fmt/format.h>
#include "polarGB/graphics_display.h"


GraphicsDisplay::GraphicsDisplay()
{
}


GraphicsDisplay::~GraphicsDisplay()
{
}


/**
 * Starts up a SDL2 and create a window.
 * Return value:
 *  - 0 on success.
 *  - 1 on error.
 */
int GraphicsDisplay::startUp()
{
    this->windowName = "polarGB";
    this->width = 800;
    this->height = 600;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fmt::print(stderr, "Could not initialize SDL, reason: {}\n", SDL_GetError());
        return 1;
    }

    this->window = SDL_CreateWindow(this->windowName.c_str(),
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    this->width,
                                    this->height,
                                    0);

    if(window == NULL)
    {
        fmt::print(stderr, "Could not create window, reason: {}\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    return 0;
}


void GraphicsDisplay::shutDown()
{
    SDL_DestroyWindow(this->window);
    this->window = nullptr;
    SDL_Quit();
}


void GraphicsDisplay::drawFrame()
{

}
