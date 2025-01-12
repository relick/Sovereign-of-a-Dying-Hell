#include "Worlds.hpp"
#include "Game.hpp"
#include "Version.hpp"
#include "PaletteOps.hpp"

#include <genesis.h>
#include "res_bg.h"
#include "res_fonts.h"

#include <cstring>

namespace Game
{

void VBlank_TextFrameReset()
{
	// Delay the reset til top of screen
	while(GET_VCOUNTER < 235) {}
	System::SetPalette_Fast<During::VBlank, PAL0>(beach.palette->data);
}

// current VRAM upload tile position
// TODO
u16 curTileInd = TILE_USER_INDEX;

void GameWorld::Init
(
	Game& io_game
)
{
	VDP_setWindowVPos(true, c_textFramePos);
	VDP_setTextPlane(VDPPlane::WINDOW);
	VDP_setTextPriority(0);

	VDP_setTextPalette(PAL3);
	VDP_loadFont(&vn_font, TransferMethod::DMA);

	VDP_drawImageEx(VDPPlane::BG_B, &beach, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, curTileInd), 0, 0, false, DMA);
	curTileInd += beach.tileset->numTile;

	// Show image-based text frame
	/*if (curTileInd + text_frame.tileset->numTile < userTileMaxIndex)
	{
		VDP_drawImageEx(VDP_getTextPlane(), &text_frame, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, curTileInd), 0, c_textFramePos, false, DMA);
		curTileInd += text_frame.tileset->numTile;
	}*/

	PAL_setColorsDMA(0, palette_black, 64);
	u16 fullPal[64] = { 0 };

	std::memcpy(fullPal, beach.palette->data, 16 * sizeof(u16));
	std::memcpy(fullPal + 48, text_frame.palette->data + 48, 16 * sizeof(u16));
	PAL_fadeToAll(fullPal, FramesPerSecond(), false);

	// Show palette-based text frame
	SetTextFramePalette(beach_frame_pal);
	SYS_setHIntCallback(&HInt_TextFrameDMA<184>);
	SYS_setVBlankCallback(&VBlank_TextFrameReset);
	VDP_setHInterrupt(TRUE);
	VDP_setHIntCounter(0);

	m_printer.PushLine("The quick brown fox, jumps over the lazy dog. \n\"Amazing'!??! (3*3+2); or: £3.50 :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :) :)");
}

void GameWorld::Shutdown
(
	Game& io_game
)
{
}

void GameWorld::Run
(
	Game& io_game
)
{
	u16 buttons = JOY_readJoypad(JOY_1);

	static bool pressed = false;
	if (buttons != 0)
	{
		if (!pressed)
			m_printer.Next();
		pressed = true;
	}
	else
	{
		pressed = false;
	}

	static u16 time = 0;
	if(time == 3)
	{
		m_printer.Update();
		time = 0;
	}
	else
	{
		++time;
	}

}

}