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
#include <assert.h>
#include <cstring>
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
    this->width = SCREEN_WIDTH;
    this->height = SCREEN_HEIGHT;

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

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        fmt::print(stderr, "Could not create renderer, reason: {}\n", SDL_GetError());
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
    }

    SDL_SetRenderDrawColor(this->renderer, 0xff, 0xff, 0xff, 0xff);

    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    pixels = new u8[width * height * 4];
    std::memset(pixels, 0x0, width * height * 4);
    texturePixels = NULL;

    return 0;
}


void GraphicsDisplay::shutDown()
{
    delete[] pixels;
    pixels = nullptr;

    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;

    SDL_DestroyWindow(this->window);
    this->window = nullptr;
    SDL_Quit();
}


void GraphicsDisplay::drawFrame()
{
    lockTexture();
    copyPixelsToTexture();
    unlockTexture();

    /* Clear the screen. */
    SDL_RenderClear(this->renderer);

    /* Render ?? */
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);

    /* Update screen. */
    SDL_RenderPresent(this->renderer);
}


/**
 * Pixel data is stored in ABGR8888 format.
 */
void GraphicsDisplay::updatePixel(u8 x, u8 y, u8 r, u8 g, u8 b, u8 a)
{
    assert(x < this->width);
    assert(y < this->height);

    int index = (y * 4 * this->width) + (4 * x);
    this->pixels[index] = r;
    this->pixels[index + 1] = g;
    this->pixels[index + 2] = b;
    this->pixels[index + 3] = a;
}


bool GraphicsDisplay::lockTexture()
{
    /* Check if texture is already locked. */
    if(this->texturePixels != NULL)
        return false;

    if(SDL_LockTexture(this->texture, NULL, &this->texturePixels, &this->pitch) != 0)
    {
        fmt::print(stderr, "Could not lock texture, reason: {}\n", SDL_GetError());
        return false;
    }

    return true;
}


void GraphicsDisplay::unlockTexture()
{
    SDL_UnlockTexture(this->texture);
    this->texturePixels = NULL;
    this->pitch = 0;
}


void GraphicsDisplay::copyPixelsToTexture()
{
    assert(this->texturePixels != NULL);
    assert(this->pixels != NULL);

    std::memcpy(this->texturePixels, this->pixels, this->pitch * this->height);
}
