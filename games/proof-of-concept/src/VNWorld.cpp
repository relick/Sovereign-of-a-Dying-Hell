#include "VNWorld.hpp"
#include "Game.hpp"
#include "Version.hpp"
#include "PaletteOps.hpp"
#include "Script.hpp"

#include <genesis.h>
#include "res_fonts.h"

#include <cstring>

namespace Game
{

static u16 const* s_bgNormalPal{ palette_black };
static u16 const* s_bgNamePal{ palette_black };
static u16 const* s_bgTextPal{ palette_black };

static u16 const* s_charaNormalPal{ palette_black };
static u16 const* s_charaNamePal{ palette_black };
static u16 const* s_charaTextPal{ palette_black };

//------------------------------------------------------------------------------
void HInt_TextArea_SetName();
void HInt_TextArea_SetText();
void HInt_TextArea_Reset();

void HInt_TextArea_SetName()
{
	SetBGTextFramePalette(s_bgNamePal);
	SetCharTextFramePalette(s_charaNamePal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, false, c_textFramePos * 8 - 20, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	SetBGTextFramePalette(s_bgTextPal);
	SetCharTextFramePalette(s_charaTextPal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 + 0, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	SetBGTextFramePalette(s_bgNormalPal);
	SetCharTextFramePalette(s_charaNormalPal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8, &HInt_TextArea_SetName>);
}

//------------------------------------------------------------------------------
VNWorld::VNWorld
(
	std::unique_ptr<Script>&& i_script
)
	: m_script{ std::move(i_script) }
{
	if (!m_script) { Error("Must provide a script to VNWorld"); }
}

// Based on VDP_drawImageEx
bool FastImageLoad(VDPPlane plane, const Image* image, u16 basetile, u16 x, u16 y)
{
	if (!VDP_loadTileSet(image->tileset, basetile & TILE_INDEX_MASK, DMA))
	{
		return false;
	}

	if (!VDP_setTileMapEx(plane, image->tilemap, basetile, x, y, 0, 0, image->tilemap->w, image->tilemap->h, DMA))
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
WorldRoutine VNWorld::Init
(
	Game& io_game
)
{
	// Enable shadow effects on text
	VDP_setHilightShadow(1);

	// Show palette-based text frame
	HInt_TextArea_SetName();
	VDP_setHIntCounter(0);

	m_printer.Init(io_game, vn_font, name_font);

	u16 blackWithTextPal[64] = { 0 };
	std::memcpy(blackWithTextPal + 48, text_font_pal.data, 16 * sizeof(u16));
	PAL_setColors(0, blackWithTextPal, 64, DMA_QUEUE_COPY);

	// Wait a frame for colours to swap
	co_yield {};

	// Fill with reserved but highlighted empty tile
	//VDP_fillTileMap(VDP_BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 2047), 0, 64 * 32);

	m_script->Init(io_game, *this, m_characters);

	VDP_setHInterrupt(true);

	m_nextBGCallbackID = io_game.AddVBlankCallback(
		[this]
		{
			if (PAL_isDoingFade())
			{
				return;
			}

			if (m_nextBG)
			{
				FastImageLoad(VDPPlane::BG_B, m_nextBG, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0), 0, 0);
				PAL_fadeInPalette(PAL0, m_nextBG->palette->data, FramesPerSecond() / 4, true);
				s_bgNormalPal = m_nextBG->palette->data;
				s_bgNamePal = m_nextBG->palette->data;
				s_bgTextPal = m_nextBG->palette->data;
				m_nextBG = nullptr;
			}
		}
	);

	m_nextPoseCallbackID = io_game.AddVBlankCallback(
		[this]
		{
			if (PAL_isDoingFade())
			{
				return;
			}

			if (m_nextPose)
			{
				// Fill with reserved but highlighted empty tile
				//VDP_fillTileMap(VDP_BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 2047), 0, 64 * 32);
				FastImageLoad(BG_A, m_nextPose->m_image, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - m_nextPose->m_image->tileset->numTile), 0, 0);
				//PAL_fadeInPalette(PAL1, m_nextPose->m_image->palette->data, FramesPerSecond() / 4, true);
				PAL_setColors(PAL1 * 16, m_nextPose->m_image->palette->data, 16, DMA);
				s_charaNormalPal = m_nextPose->m_image->palette->data;
				s_charaNamePal = m_nextPose->m_namePal->data;
				s_charaTextPal = m_nextPose->m_textPal->data;
				m_nextPose = nullptr;
			}
		}
	);

	co_return;
}

//------------------------------------------------------------------------------
WorldRoutine VNWorld::Shutdown
(
	Game& io_game
)
{
	io_game.RemoveVBlankCallback(m_nextBGCallbackID);
	io_game.RemoveVBlankCallback(m_nextPoseCallbackID);

	SYS_setHIntCallback(nullptr);
	VDP_setHInterrupt(false);
	m_printer.Shutdown();

	co_return;
}

//------------------------------------------------------------------------------
void VNWorld::Run
(
	Game& io_game
)
{
	if (PAL_isDoingFade() || m_nextPose || m_nextBG)
	{
		return;
	}

	VDP_setHInterrupt(true);

	u16 const buttons = JOY_readJoypad(JOY_1);

	static bool pressed = false;
	if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
	{
		if (!pressed)
		{
			if (m_readyForNext)
			{
				m_script->Update(io_game, *this);
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

//------------------------------------------------------------------------------
void VNWorld::StartMusic
(
	u8 const* i_bgm,
	u16 i_fadeInFrames,
	bool i_loop
)
{
	//XGM_startPlay(i_bgm);
	//XGM_setLoopNumber(i_loop ? -1 : 0);
}

//------------------------------------------------------------------------------
void VNWorld::StopMusic
(
	u16 i_fadeOutFrames
)
{
	XGM_stopPlay();
}

//------------------------------------------------------------------------------
void VNWorld::SetBG
(
	Image const& i_bg
)
{
	m_nextBG = &i_bg;
	VDP_setHInterrupt(false);
	PAL_fadeOutPalette(PAL0, FramesPerSecond() / 4, true);
}

//------------------------------------------------------------------------------
void VNWorld::BlackBG()
{
	VDP_setHInterrupt(false);
	PAL_fadeOutPalette(PAL0, FramesPerSecond() / 4, true);
}

//------------------------------------------------------------------------------
void VNWorld::SetCharacter
(
	char const* i_charName,
	char const* i_poseName
)
{
	auto const [_, pose] = m_characters.FindPose(i_charName, i_poseName);
	if (pose)
	{
		m_nextPose = pose;
		//VDP_setHInterrupt(false);
		//PAL_fadeOutPalette(PAL1, FramesPerSecond() / 4, true);
	}
}

//------------------------------------------------------------------------------
void VNWorld::HideCharacter()
{
	// Fill with reserved but highlighted empty tile
	//VDP_fillTileMap(VDP_BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 2047), 0, 64 * 32);
}

//------------------------------------------------------------------------------
void VNWorld::SetCharName
(
	char const* i_charName
)
{
	auto const chara = m_characters.FindCharacter(i_charName);
	if (chara)
	{
		m_printer.SetName(chara->m_displayName, chara->m_showOnLeft);
	}
	else
	{
		m_printer.SetName(nullptr, true);
	}
}

//------------------------------------------------------------------------------
void VNWorld::SetText
(
	char const* i_text
)
{
	m_printer.SetText(i_text);
}

}