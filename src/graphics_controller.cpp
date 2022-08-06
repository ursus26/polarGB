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

#include <cassert>
#include <fmt/format.h>
#include "polarGB/graphics_controller.h"


const u16 TILE_DATA_AREA_1 = 0x800;
const u16 TILE_DATA_AREA_2 = 0;
const u16 TILE_MAP_AREA_1 = 0x9800 - 0x8000;
const u16 TILE_MAP_AREA_2 = 0x9c00 - 0x8000;


GraphicsController::GraphicsController(std::shared_ptr<InterruptController> ic, bool noWindow)
{
    assert(ic != nullptr);

    /* Display registers */
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

    /* Memory */
    this->vram.size = 0x2000;
    this->vram.mem = new u8[vram.size]();
    this->oam = {};

    this->mode = 2;
    this->modeCycles = 0;
    this->noWindow = noWindow;
    this->display = nullptr;
    this->interruptController = ic;

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


GraphicsController::~GraphicsController()
{
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
                searchForObjectsOnCurrentScanline();
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

            processObjectPixel(i);
        }

    }
    else
    {
        for(auto i = 0; i < SCREEN_WIDTH; i++)
            this->display->updatePixel(i, this->LY, 0xff, 0xff, 0xff, 0xff);
    }
}


/**
 * Process the background or windows pixel. The pixel coordinate is (x, LY) where x is given as
 * an argument and the y-pos is the current scanline.
 */
void GraphicsController::processBackgroundPixel(u8 x)
{
    assert(LCDC & 0x1);
    assert((LCDC & 0x80) == 0x80);

    u16 backgroundTileMapAddr = TILE_MAP_AREA_1;
    if(this->LCDC & 0x8)
        backgroundTileMapAddr = TILE_MAP_AREA_2;

    u16 windowTileMapAddr = TILE_MAP_AREA_1;
    if(this->LCDC & 0x40)
        windowTileMapAddr = TILE_MAP_AREA_2;

    u16 tileDataAddr = TILE_DATA_AREA_1;
    if(this->LCDC & 0x10)
        tileDataAddr = TILE_DATA_AREA_2;

    int xPixel, yPixel, tileDataIndex;
    bool windowEnabled = this->LCDC & 0x20;

    if(windowEnabled && x >= (this->WX - 6) && this->LY >= this->WY)
    {
        /* Window */

        /* Get window coordinate. */
        xPixel = this->WX - x;
        yPixel = this->WY - this->LY;

        /* Get tile id. */
        int tileMapIndex = getTileIdx(xPixel, yPixel);
        tileDataIndex = this->vramRead(windowTileMapAddr + tileMapIndex);
    }
    else
    {
        /* Background */

        /* Get background coordinate. */
        xPixel = ((int)SCX + (int)x) % 256;
        yPixel = ((int)LY + (int)SCY) % 256;

        /* Get tile id. */
        int tileMapIndex = getTileIdx(xPixel, yPixel);
        tileDataIndex = this->vramRead(backgroundTileMapAddr + tileMapIndex);
    }

    /* Fetch the pixel shade; */
    int xBlock = 7 - (xPixel % 8);
    int yBlock = yPixel % 8;
    u16 addr = tileDataAddr + (tileDataIndex * 16) + (yBlock * 2);
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

/**
 * Process a pixel in the object layer. This function might not set a pixel if there are no sprites
 * located on the specific screen pixel. The pixel we are processing is (x,LY).
 */
void GraphicsController::processObjectPixel(u8 x)
{

    const u8 xOffset = 8;
    const u8 spriteWidth = 8;

    for(auto objectAttribute : this->objectsOnCurrentScanline)
    {
        /* Continue searching if x position is too small. */
        if(objectAttribute.x - xOffset + spriteWidth < x)
            continue;

        /* Stop searching for the object when they are past the current pixel. (List is sorted on x-axis) */
        if(objectAttribute.x - xOffset > x)
            break;

        /* Fetch the pixel shade; */
        int xBlock = 7 - (x % 8);
        int yBlock = LY % 8;
        u16 addr = (objectAttribute.tileIndex * 16) + (yBlock * 2);
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


void GraphicsController::searchForObjectsOnCurrentScanline()
{
    /* Clear objects from previous scanline. */
    this->objectsOnCurrentScanline.clear();

    int offsettedScanline = this->LY + 16;
    for(auto spriteAttribute = this->oam.begin(); spriteAttribute != this->oam.end(); ++spriteAttribute)
    {
        if(spriteAttribute->y <= offsettedScanline && spriteAttribute->y + 8 > offsettedScanline)
            this->objectsOnCurrentScanline.push_back(*spriteAttribute);
    }

    this->objectsOnCurrentScanline.sort(compareSpriteAttributesByXCoordinate);
}


bool compareSpriteAttributesByXCoordinate(SpriteAttributes first, SpriteAttributes second)
{
    if(first.x < second.x)
        return true;
    else
        return false;
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
    u16 attributeIndex = address >> 2;
    SpriteAttributes attribute = this->oam.at(attributeIndex);

    // u16 byteIndex = address & 0x3;
    switch(address & 0x3) {
        case 0:
            return attribute.y;
        case 1:
            return attribute.x;
        case 2:
            return attribute.tileIndex;
        case 3:
            return attribute.flags;
        default:
            return 0;
    }
}


void GraphicsController::oamWrite(u16 address, u8 data)
{
    u16 attributeIndex = address >> 2;
    u16 byteIndex = address & 0x3;
    switch(byteIndex) {
        case 0:
            this->oam.at(attributeIndex).y = data;
            break;
        case 1:
            this->oam.at(attributeIndex).x = data;
            break;
        case 2:
            this->oam.at(attributeIndex).tileIndex = data;
            break;
        case 3:
            this->oam.at(attributeIndex).flags = data;
            break;
    }
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
