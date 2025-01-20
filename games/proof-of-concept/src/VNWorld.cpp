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
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, false, c_textFramePos * 8 - 20, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	SetBGTextFramePalette(beach2_text_pal);
	SetCharTextFramePalette(stacey_text_pal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 + 0, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	SetBGTextFramePalette(*(beach2.palette));
	SetCharTextFramePalette(*(stacey.palette));
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8, &HInt_TextArea_SetName>);
}

void VNWorld::Init
(
	Game& io_game
)
{
	VDP_drawImageEx(VDPPlane::BG_B, &beach2, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0), 0, 0, false, true);
	VDP_drawImageEx(VDPPlane::BG_A, &stacey, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - stacey.tileset->numTile), 0, 0, false, true);

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

	m_sceneRun = m_scene.Run(io_game, m_printer);
}

void VNWorld::Shutdown
(
	Game& io_game
)
{
	SYS_setHIntCallback(nullptr);
	VDP_setHInterrupt(false);
	m_printer.Shutdown();
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
			VDP_setHInterrupt(true);
			//m_printer.SetText("Wow...\nI've never been to the beach before.\nLet's have some fun!\nWe could even have a barbeque!");
			//m_printer.SetText("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
			//m_printer.SetName("STACEY", false);
			
			// Start the scene
			m_sceneRun.resume();
		}
		else
		{
			return;
		}
	}

	u16 const buttons = JOY_readJoypad(JOY_1);

	static bool pressed = false;
	if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
	{
		if (!pressed)
		{
			if(m_readyForNext)
			{
				m_sceneRun.resume();
				m_readyForNext = false;
			}
			else
			{
				m_printer.Next();
			}
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
		if (m_printer.Update())
		{
			m_readyForNext = true;
		}
		time = 0;
	}
	else
	{
		++time;
	}

}

}