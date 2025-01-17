#include "Worlds.hpp"
#include "Game.hpp"
#include "Version.hpp"
#include "PaletteOps.hpp"

#include <genesis.h>
#include "res_bg.h"
#include "res_fonts.h"
#include "res_music.h"

#include <cstring>

namespace Game
{

void HInt_TextArea_SetName();
void HInt_TextArea_SetText();
void HInt_TextArea_Reset();

void HInt_TextArea_SetName()
{
	SetTextFramePalette(beach_name_pal);
	SYS_setHIntCallback(&HInt_TextFrameDMA<c_textFramePos * 8 - 14, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	SetTextFramePalette(beach_text_pal);
	SYS_setHIntCallback(&HInt_TextFrameDMA<c_textFramePos * 8 + 4, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	SetTextFramePalette(*(beach.palette));
	SYS_setHIntCallback(&HInt_TextFrameDMA<(c_textFramePos + c_textFrameHeight) * 8, &HInt_TextArea_SetName>);
}

// current VRAM upload tile position
// TODO
u16 curTileInd = TILE_USER_INDEX;

void VNWorld::Init
(
	Game& io_game
)
{
	VDP_setTextPlane(VDPPlane::BG_A);

	VDP_drawImageEx(VDPPlane::BG_B, &beach, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, curTileInd), 0, 0, false, DMA);
	curTileInd += beach.tileset->numTile;

	PAL_setColorsDMA(0, palette_black, 64);
	u16 fullPal[64] = { 0 };

	std::memcpy(fullPal, beach.palette->data, 16 * sizeof(u16));
	std::memcpy(fullPal + 48, text_font_pal.data, 16 * sizeof(u16));
	PAL_fadeToAll(fullPal, FramesPerSecond(), false);

	// Show palette-based text frame
	HInt_TextArea_SetName();
	VDP_setHInterrupt(TRUE);
	VDP_setHIntCounter(0);

	m_printer.Init(vn_font, name_font);
	m_printer.SetText("Wow...\nI've never been to the beach before.\nLet's have some fun!");
	m_printer.SetName("WAKISAGIHIME", false);

	// Enable shadow effects on text
	VDP_setHilightShadow(1);

	// Playing music really is this easy
	// XGM_startPlay(spacey);
}

void VNWorld::Shutdown
(
	Game& io_game
)
{
}

void VNWorld::Run
(
	Game& io_game
)
{
	u16 buttons = JOY_readJoypad(JOY_1);

	static bool pressed = false;
	if (buttons != 0)
	{
		if (!pressed)
		{
			m_printer.Next();
		}
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