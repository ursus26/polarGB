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

#ifndef VIDEO_H
#define VIDEO_H

#include <string>
#include "types.h"


typedef enum VideoRegister
{
    RegLCDC,
    RegSTAT,
    RegSCY,
    RegSCX,
    RegLY,
    RegLYC,
    RegDMA,
    RegBGP,
    RegOBP0,
    RegOBP1,
    RegWY,
    RegWX
} VideoRegister;


class Video
{
public:
    Video();
    ~Video();

    /* Initialization and clean up. */
    void startUp(bool noWindow);
    void shutDown();

    /* Video RAM read and write. */
    u8 vramRead(u16 address);
    void vramWrite(u16 address, u8 data);
    u8 videoRegisterRead(VideoRegister reg);
    void videoRegisterWrite(VideoRegister reg, u8 data);

    void update(u8 cycles);

private:
    Ram vram;
    u8 LCDC;
    u8 STAT;
    u8 SCY;
    u8 SCX;
    u8 LY;
    u8 LYC;
    u8 DMA;
    u8 BGP;
    u8 OBP0;
    u8 OBP1;
    u8 WY;
    u8 WX;

    /* OLD REGISTERS */
    // u8 scanline;
    u8 mode;
    u64 modeCycles;

    bool noWindow;  /* Mainly used for testing in order to not setup OpenGL. */
    std::string windowName;
    int width;
    int height;

    void drawFrame();
    void updateMatchFlag();

    void setCurrentMode(u8 newMode);
    // void setCurrentScanline(u8 lineIdx);
};


#endif /* VIDEO_H */
