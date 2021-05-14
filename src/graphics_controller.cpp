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
#include "polarGB/graphics_controller.h"


const u16 TILE_DATA_AREA_1 = 0x800;
const u16 TILE_DATA_AREA_2 = 0;
const u16 TILE_MAP_AREA_1 = 0x9800 - 0x8000;
const u16 TILE_MAP_AREA_2 = 0x9c00 - 0x8000;


GraphicsController::GraphicsController()
{
    this->LCDC = 0;
    this->STAT = 0;
    this->SCY = 0;
    this->SCX = 0;
    this->LY = 0;
    this->LYC = 0;
    this->DMA = 0;
    this->BGP = 0;
    this->OBP0 = 0;
    this->OBP1 = 0;
    this->WY = 0;
    this->WX = 0;
    this->vram.size = 0;
    this->vram.mem = nullptr;
    this->oam = {0};
    this->mode = 2;
    this->modeCycles = 0;
    this->noWindow = false;
    this->interruptController = nullptr;
    this->display = nullptr;
}


GraphicsController::~GraphicsController()
{
}


void GraphicsController::startUp(InterruptController* interruptController, bool noWindow)
{
    assert(interruptController != nullptr);

    this->noWindow = noWindow;
    this->interruptController = interruptController;

    vram.size = 0x2000;
    vram.mem = new u8[vram.size]();

    this->oam = {0};

    this->mode = 2;
    this->modeCycles = 0;

    if(noWindow == false)
    {
        this->display = new GraphicsDisplay();
        int status = this->display->startUp();
        if(status != 0)
        {
            this->noWindow = true;
            delete display;
            this->display = nullptr;
        }
    }
}


void GraphicsController::shutDown()
{
    delete[] vram.mem;
    vram.mem = nullptr;
    vram.size = 0;

    if(noWindow == false)
    {
        display->shutDown();
        delete display;
        display = nullptr;
        this->noWindow = true;
    }

    this->interruptController = nullptr;
}


/**
 * Updates the video side of the GB by a given number of cpu cycles.
 * Inspiration and source: http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
 */
void GraphicsController::update(u8 cycles)
{
    this->modeCycles += cycles;

    switch(this->mode)
    {
        /* Horizontal blanking. */
        case 0:
            if(modeCycles >= 51)
            {
                modeCycles -= 51;
                LY++;

                /* Check if we enter V-Blank or go to mode 2. */
                if(LY == 144)
                {
                    setCurrentMode(1);
                    this->display->drawFrame();
                    interruptController->requestInterrupt(int_vblank);

                    if(STAT & 0x10)
                        interruptController->requestInterrupt(int_stat);
                }
                else
                {
                    setCurrentMode(2);

                    if(STAT & 0x20)
                        interruptController->requestInterrupt(int_stat);
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
                    if(STAT & 0x20)
                        interruptController->requestInterrupt(int_stat);
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
                processScanline();

                if(STAT & 0x8)
                    interruptController->requestInterrupt(int_stat);
            }
            break;
    }

    /* Update the match flag in the STAT register. */
    updateMatchFlag();
}


/**
 * Transforms a pixel coordinate to a tile coordinate and then calculate the tile index in memory.
 * The transformation will make sure that the pixel can be divided by 8 bits by removing bits 0-2.
 */
constexpr int getTileIdx(u8 x, u8 y)
{
    return ((x & 0xf8) / 0x8) + 32 * ((y & 0xf8) / 0x8);
}


void GraphicsController::processScanline()
{
    bool LCDEnabled = (this->LCDC & 0x80) == 0x80;

    if(LCDEnabled)
    {
        bool backgroundAndWindowEnabled = this->LCDC & 0x1;
        for(int i = 0; i < SCREEN_WIDTH; i++)
        {
            if(backgroundAndWindowEnabled)
                processBackgroundPixel(i);
            else
                this->display->updatePixel(i, this->LY, 0xff, 0xff, 0xff, 0xff);

            processObjectPixel();
        }

    }
    else
    {
        for(auto i = 0; i < SCREEN_WIDTH; i++)
            this->display->updatePixel(i, this->LY, 0xff, 0xff, 0xff, 0xff);
    }
}


void GraphicsController::processBackgroundPixel(u8 x)
{
    assert(LCDC & 0x1);
    assert((LCDC & 0x80) == 0x80);

    u16 backgroundTileMapAddr = TILE_MAP_AREA_1;
    if(this->LCDC & 0x8)
        backgroundTileMapAddr = TILE_MAP_AREA_2;
    u16 bgBaseAddr = TILE_DATA_AREA_1;
    if(this->LCDC & 0x10)
        bgBaseAddr = TILE_DATA_AREA_2;

    if(this->LCDC & 0x20 && x >= (this->WX - 6) && this->LY >= this->WY)
    {
        /* Window */
    }
    else
    {
        /* Background */

        /* Get background coordinate. */
        int xPixel = (this->SCX + x) % 256;
        int yPixel = ((int)this->LY + (int)this->SCY) % 256;
        // int yPixel = this->LY;

        /* Get tile id. */
        int tileIdx = getTileIdx(xPixel, yPixel);
        int chrCode = this->vramRead(backgroundTileMapAddr + tileIdx);

        // fmt::print("LCDC: {:x}, x: {}, y: {}, map_addr: {:x}, tile_index: {:x}, tile address: {:x}\n", this->LCDC, x, this->LY, backgroundTileMapAddr + tileIdx, chrCode, bgBaseAddr + (16 * chrCode));

        /* Fetch the pixel shade; */
        int xBlock = 8 - (xPixel % 8);
        int yBlock = yPixel % 8;
        u16 addr = bgBaseAddr + (chrCode * 16) + (yBlock * 2);
        u8 low = this->vramRead(addr);
        u8 high = this->vramRead(addr + 1);
        u8 pixelShade = ((low >> xBlock) & 0x1) | (((high >> xBlock) & 0x1) << 1);

        u8 color = 0;
        switch (pixelShade) {
            case 0:
                color = 255;
                break;
            case 1:
                color = 170;
                break;
            case 2:
                color = 85;
                break;
            case 3:
                color = 0;
                break;
        }

        this->display->updatePixel(x, this->LY, color, color, color, 0xff);
    }
}


void GraphicsController::processObjectPixel()
{
}


/**
 * Switch to a new display mode and push this update to the corresponding STAT display register.
 * Mode can only take the values: 0, 1, 2 or 3.
 */
void GraphicsController::setCurrentMode(u8 newMode)
{
    assert(newMode < 4);

    /* Update the mode locally. */
    this->mode = newMode;

    /* Update the mode in the STAT register. */
    this->STAT = (this->STAT & 0xfc) | newMode;
}


void GraphicsController::updateMatchFlag()
{
    if(LY == LYC)
        STAT |= 0x4;
    else
        STAT &= ~0x4;

    if(STAT & 0x40 && STAT & 0x4)
        interruptController->requestInterrupt(int_stat);
}


u8 GraphicsController::vramRead(u16 address)
{
    assert(address < vram.size);
    assert(vram.mem != nullptr);

    return vram.mem[address];
}


void GraphicsController::vramWrite(u16 address, u8 data)
{
    assert(address < vram.size);
    assert(vram.mem != nullptr);

    vram.mem[address] = data;
}


u8 GraphicsController::oamRead(u16 address)
{
    return this->oam.at(address);
}


void GraphicsController::oamWrite(u16 address, u8 data)
{
    this->oam.at(address) = data;
}


u8 GraphicsController::displayRegisterRead(displayRegister_t reg)
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


void GraphicsController::displayRegisterWrite(displayRegister_t reg, u8 data)
{
    switch(reg) {
        case RegLCDC:
            LCDC = data;
            break;
        case RegSTAT:
            STAT = data & 0x7c;
            break;
        case RegSCY:
            SCY = data;
            break;
        case RegSCX:
            SCX = data;
            break;
        case RegLY:
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
