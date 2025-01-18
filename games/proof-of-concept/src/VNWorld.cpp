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

#define LINETABLE_SIZE 224
static s16 LineTable[LINETABLE_SIZE] = {0};
static u16 SineScroll = 0;

// Called once every frame in main loop
void FX_SineWave()
{
	for (u8 i = 0; i < LINETABLE_SIZE; i++)
	{
		LineTable[i] = sinFix16(i + SineScroll);
	}

	SineScroll += 4;
}

// Called once every frame in vblank
void FX_UpdateScroll()
{
	VDP_setHorizontalScrollLine(BG_A, 0, LineTable, LINETABLE_SIZE, DMA);
}

// From VDP_drawImageEx
bool DrawImage_TMNoPalette(VDPPlane plane, const Image *image, u16 basetile, u16 x, u16 y, TransferMethod tm)
{
	if (!VDP_loadTileSet(image->tileset, basetile & TILE_INDEX_MASK, tm))
	{
		return false;
	}

	TileMap *tilemap = image->tilemap;

	if (!VDP_setTileMapEx(plane, tilemap, basetile, x, y, 0, 0, tilemap->w, tilemap->h, tm))
	{
		return false;
	}

	return true;
}

void VNWorld::Init
(
	Game& io_game
)
{
	DrawImage_TMNoPalette(VDPPlane::BG_B, &beach2, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, myTileInd), 0, 0, DMA);
	myTileInd += beach2.tileset->numTile;
	DrawImage_TMNoPalette(VDPPlane::BG_A, &stacey, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, myTileInd), 0, 0, DMA);
	myTileInd += stacey.tileset->numTile;

	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
	m_fxScrollID = io_game.AddVBlankCallback(FX_UpdateScroll);

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

	m_printer.Init(vn_font, name_font);

	// Playing music really is this easy
	// XGM_startPlay(spacey);
}

void VNWorld::Shutdown
(
	Game& io_game
)
{
	io_game.RemoveVBlankCallback(m_fxScrollID);
}

void VNWorld::Run
(
	Game& io_game
)
{
	FX_SineWave();

	if (m_fading)
	{
		if(!PAL_isDoingFade())
		{
			m_fading = false;
			VDP_setHInterrupt(TRUE);
			m_printer.SetText("Wow...\nI've never been to the beach before.\nLet's have some fun!");
			m_printer.SetName("STACEY", false);
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