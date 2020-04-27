## Video Registers

There are in total 12 video registers in the DMG and an additional 9 in the CGB. Furthermore the
Game Boy programming manual states that OAM is also a register that can hold data for 40 objects.
The registers in the DMG are:

|Address    |Register name  |Info                                               |
|-----------|---------------|---------------------------------------------------|
|0xff40     |LCDC           |LCD Control Register                               |
|0xff41     |STAT           |LCD Status Flag                                    |
|0xff42     |SCY            |Scroll Y                                           |
|0xff43     |SCX            |Scroll X                                           |
|0xff44     |LY             |LCDC y-coordinate                                  |
|0xff45     |LYC            |LY Compare                                         |
|0xff46     |DMA            |Direct Memory Access Transfer and Starting Address |
|0xff47     |BGP            |Background Palette Data                            |
|0xff48     |OBP0           |Object Palette Data 0                              |
|0xff49     |OBP1           |Object Palette Data 1                              |
|0xff4a     |WY             |Window y-coordinate                                |
|0xff4b     |WX             |Window x-coordinate                                |

and the additional registers in the CGB are:

|Address    |Register name  |Info   |
|-----------|---------------|-------|
|0xff51     |HDMA1          |       |
|0xff52     |HDMA2          |       |
|0xff53     |HDMA3          |       |
|0xff54     |HDMA4          |       |
|0xff55     |HDMA5          |       |
|0xff68     |BCPS           |       |
|0xff69     |BCPD           |       |
|0xff6a     |OCPS           |       |
|0xff6b     |OCPD           |       |

And lastly there is OAM which stores data for 40 objects. OAM is a memory area located at address
range 0xfe00 - 0xfe9f.


## Rendering layers
The Game Boy has 3 distinct layers that together create the graphics for games. These are:
**background**, **window**, and **objects**. The background is self explanatory. It is the
background of a game. The window is similar to the background but it is placed on top of the
background. Most games use this layer for UI. The last layer contains objects which are more like
sprites. They are placed on top of everything else.
