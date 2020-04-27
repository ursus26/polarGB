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

#ifndef GRAPHICS_DISPLAY_H
#define GRAPHICS_DISPLAY_H


#include <string>
#include <SDL2/SDL.h>
#include "types.h"


const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;

class GraphicsDisplay
{

public:
    GraphicsDisplay();
    ~GraphicsDisplay();

    int startUp();
    void shutDown();

    void updatePixel(u8 x, u8 y, u8 r, u8 g, u8 b, u8 a);
    void drawFrame();

private:
    std::string windowName;
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    u8* pixels;
    void* texturePixels;
    int pitch;

    bool lockTexture();
    void unlockTexture();
    void copyPixelsToTexture();
};


#endif /* GRAPHICS_DISPLAY_H */
