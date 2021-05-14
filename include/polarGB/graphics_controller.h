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

#ifndef GRAPHICS_CONTROLLER_H
#define GRAPHICS_CONTROLLER_H

#include <string>
#include "types.h"
#include "interrupt_controller.h"
#include "graphics_display.h"


typedef enum DisplayRegister
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
} displayRegister_t;


class GraphicsController
{
public:
    GraphicsController();
    ~GraphicsController();

    /* Initialization and clean up. */
    void startUp(InterruptController* interruptController, bool noWindow);
    void shutDown();

    /* Video RAM read and write. */
    u8 vramRead(u16 address);
    void vramWrite(u16 address, u8 data);
    u8 displayRegisterRead(displayRegister_t reg);
    void displayRegisterWrite(displayRegister_t reg, u8 data);

    void update(u8 cycles);

private:
    ram_t vram;
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

    /* Member variables */
    u8 mode;
    u64 modeCycles;

    bool noWindow;  /* Mainly used for testing in order to not setup the window. */
    GraphicsDisplay* display;
    InterruptController* interruptController;

    // void drawFrame();
    void updateMatchFlag();

    void setCurrentMode(u8 newMode);
    void processScanline();
    void processBackgroundPixel(u8 x);
    void processObjectPixel();
};


#endif /* GRAPHICS_CONTROLLER_H */
