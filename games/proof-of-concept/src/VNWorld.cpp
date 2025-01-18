#include "Worlds.hpp"
#include "Game.hpp"
#include "Version.hpp"
#include "PaletteOps.hpp"

#include <genesis.h>
#include "res_bg.h"
#include "res_fonts.h"
#include "res_music.h"
#include "res_chara.h"

#include <cstring>

namespace Game
{

void HInt_TextArea_SetName();
void HInt_TextArea_SetText();
void HInt_TextArea_Reset();

void HInt_TextArea_SetName()
{
	SetBGTextFramePalette(beach2_name_pal);
	SetCharTextFramePalette(stacey_name_pal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, false, c_textFramePos * 8 - 15, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	SetBGTextFramePalette(beach2_text_pal);
	SetCharTextFramePalette(stacey_text_pal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 + 4, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	SetBGTextFramePalette(*(beach2.palette));
	SetCharTextFramePalette(*(stacey.palette));
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8, &HInt_TextArea_SetName>);
}

// current VRAM upload tile position
// TODO
static u16 myTileInd = TILE_USER_INDEX;

void VNWorld::Init
(
	Game& io_game
)
{
	VDP_drawImageEx(VDPPlane::BG_B, &beach2, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, myTileInd), 0, 0, false, true);
	myTileInd += beach2.tileset->numTile;
	VDP_drawImageEx(VDPPlane::BG_A, &stacey, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, myTileInd), 0, 0, false, true);
	myTileInd += stacey.tileset->numTile;

	// Enable shadow effects on text
	VDP_setHilightShadow(1);

	PAL_setColors(0, palette_black, 64, DMA);
	u16 fullPal[64] = { 0 };

	std::memcpy(fullPal, beach2.palette->data, 16 * sizeof(u16));
	std::memcpy(fullPal + 16, stacey.palette->data, 16 * sizeof(u16));
	std::memcpy(fullPal + 48, text_font_pal.data, 16 * sizeof(u16));
	PAL_fadeToAll(fullPal, FramesPerSecond(), true);
	m_fading = true;

	// Show palette-based text frame
	HInt_TextArea_SetName();
	VDP_setHIntCounter(0);

	m_printer.Init(io_game, vn_font, name_font);

	// Playing music really is this easy
	// XGM_startPlay(spacey);
}

void VNWorld::Shutdown
(
	Game& io_game
)
{
	m_printer.Shutdown(io_game);
}

void VNWorld::Run
(
	Game& io_game
)
{
	if (m_fading)
	{
		if(!PAL_isDoingFade())
		{
			m_fading = false;
			VDP_setHInterrupt(TRUE);
			m_printer.SetText("Wow...\nI've never been to the beach before.\nLet's have some fun!");
			m_printer.SetName(io_game, "STACEY", false);
		}
		else
		{
			return;
		}
	}

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