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

#include <stdlib.h>
#include <assert.h>
#include <fmt/format.h>
#include "polarGB/video.h"


Video::Video()
{
}


Video::~Video()
{
}


/**
 * Initialise GLFW and GLAD. This initialises OpenGL. We also create a Window on which we can draw
 * graphics.
 */
void Video::startUp(bool noWindow)
{
    this->noWindow = noWindow;

    vram.size = 0x2000;
    vram.mem = new u8[vram.size]();

    this->mode = 2;
    this->modeCycles = 0;

    this->windowName = "polarGB";
    this->width = 800;
    this->height = 600;
}


/**
 * Destroy the window and clean up GLFW.
 */
void Video::shutDown()
{
    // fmt::print("Video::shutDown() | Shutting down glfw\n");
    // fmt::print("Video::shutDown() | STAT: {:#x}\n", mmu->read(STAT_ADDR));
    //
    //
    // u16 start_addr = VRAM_START_ADDR;
    // if((mmu->read(STAT_ADDR) & 0x10) == 0x10)
    // {
    //     start_addr = 0x8800;
    // }
    //
    // for(int i = 0; i < 8; i++)
    // {
    //     for(int j = 0; j < 8; j++)
    //     {
    //         fmt::print("{:x} ", mmu->read(start_addr));
    //     }
    //     fmt::print("\n");
    // }

    delete[] vram.mem;
    vram.mem = nullptr;
}


/**
 * Updates the video side of the GB by a given number of cpu cycles.
 * Inspiration and source: http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
 */
void Video::update(u8 cycles)
{
    modeCycles += cycles;

    switch(this->mode)
    {
        /* Horizontal blanking. */
        case 0:
            if(modeCycles >= 60)
            {
                modeCycles -= 60;
                LY++;

                /* Check if we enter V-Blank or go to mode 2. */
                if(LY == 143)
                {
                    setCurrentMode(1);
                    drawFrame();
                }
                else
                {
                    setCurrentMode(2);
                }
            }
            break;

        /* Vertical blanking. */
        case 1:
            if(modeCycles >= 114)
            {
                modeCycles -= 114;
                LY++;

                if(LY > 153)
                {
                    setCurrentMode(2);
                    LY = 0;
                }

            }
            break;

        /* Scanning OAM */
        case 2:
            if(modeCycles >= 20)
            {
                modeCycles -= 20;
                setCurrentMode(3);
            }
            break;

        /* Reading OAM and VRAM. */
        case 3:
            if(modeCycles >= 43)
            {
                modeCycles -= 43;
                setCurrentMode(0);
            }
            break;
    }

    /* Update the match flag in the STAT register. */
    updateMatchFlag();
}


void Video::drawFrame()
{
    if(noWindow)
        return;

    // u16 start_addr = 0;
    // if((STAT & 0x10) == 0x10)
    // {
    //     start_addr = 0x800;
    // }
    //
    // for(int i = 0; i < 8; i++)
    // {
    //     for(int j = 0; j < 8; j++)
    //     {
    //         fmt::print("{:x} ", vramRead(start_addr + 0x10 * 0x2f));
    //     }
    //     fmt::print("\n");
    // }
    //
    // unsigned int sum = 0;
    // for(unsigned int i = 0; i < vram.size; i++)
    // {
    //     u8 data = vramRead(i);
    //     sum += data;
    //     if(data > 0)
    //     {
    //         fmt::print("FOUND VRAM DATA AT: VRAM[{:#x}] = {:#x}\n", i, data);
    //     }
    // }
    //
    // fmt::print("VRAM sum: {:#x}\n", sum);
    // fmt::print("---------------------------\n");
}


/**
 * Switch to a new display mode and push this update to the corresponding STAT display register.
 * Mode can only take the values: 0, 1, 2 or 3.
 */
void Video::setCurrentMode(u8 newMode)
{
    assert(newMode < 4);

    /* Update the mode locally. */
    this->mode = newMode;

    /* Update the mode in the STAT register. */
    this->STAT = (this->STAT & 0xfc) | newMode;
}


void Video::updateMatchFlag()
{
    if(LY == LYC)
    {
        STAT |= 0x4;
    }
    else
    {
        STAT &= ~0x4;
    }
}


u8 Video::vramRead(u16 address)
{
    assert(address < vram.size);

    return vram.mem[address];
}


void Video::vramWrite(u16 address, u8 data)
{
    assert(address < vram.size);

    vram.mem[address] = data;
}


u8 Video::videoRegisterRead(VideoRegister reg)
{
    switch(reg) {
        case RegLCDC:
            return LCDC;
        case RegSTAT:
            return STAT;
        case RegSCY:
            return SCY;
        case RegSCX:
            return SCX;
        case RegLY:
            return LY;
        case RegLYC:
            return LYC;
        case RegDMA:
            return DMA;
        case RegBGP:
            return BGP;
        case RegOBP0:
            return OBP0;
        case RegOBP1:
            return OBP1;
        case RegWY:
            return WY;
        case RegWX:
            return WX;
        default:
            return 0xff;
    }
}


void Video::videoRegisterWrite(VideoRegister reg, u8 data)
{
    switch(reg) {
        case RegLCDC:
            LCDC = data;
            break;
        case RegSTAT:
            STAT = data;
            break;
        case RegSCY:
            SCY = data;
            break;
        case RegSCX:
            SCX = data;
            break;
        case RegLY:
            LY = data;
            break;
        case RegLYC:
            LYC = data;
            break;
        case RegDMA:
            DMA = data;
            break;
        case RegBGP:
            BGP = data;
            break;
        case RegOBP0:
            OBP0 = data;
            break;
        case RegOBP1:
            OBP1 = data;
            break;
        case RegWY:
            WY = data;
            break;
        case RegWX:
            WX = data;
            break;
    }
}
