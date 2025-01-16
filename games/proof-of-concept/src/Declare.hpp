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
class DialoguePrinter;

}