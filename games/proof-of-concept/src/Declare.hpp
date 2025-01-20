#pragma once

#include <types.h>

// Specify which section of frame some code is running in for template differences
enum class During
{
	Active, // VDP actively rendering the frame
	VBlank, // VDP between frames
};

namespace Game
{

class Game;
class World;
class DialoguePrinter2;

using VBlankCallbackID = u8;

using SpriteID = u16;
class SpriteData;
class SpriteManager;

//------------------------------------------------------------------------------
enum class SpriteSize : u8
{
    r1c1 = 0b0000,
    r1c2 = 0b0100,
    r1c3 = 0b1000,
    r1c4 = 0b1100,
    r2c1 = 0b0001,
    r2c2 = 0b0101,
    r2c3 = 0b1001,
    r2c4 = 0b1101,
    r3c1 = 0b0010,
    r3c2 = 0b0110,
    r3c3 = 0b1010,
    r3c4 = 0b1110,
    r4c1 = 0b0011,
    r4c2 = 0b0111,
    r4c3 = 0b1011,
    r4c4 = 0b1111,
};

//------------------------------------------------------------------------------
enum class SpritePalette : u8
{
    Pal0 = 0,
    Pal1 = 1,
    Pal2 = 2,
    Pal3 = 3,
};

}