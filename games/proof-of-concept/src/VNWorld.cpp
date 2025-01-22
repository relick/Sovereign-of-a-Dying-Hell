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

consteval std::array<u16, 64*32> FillHilightPlaneA()
{
	std::array<u16, 64 * 32> arr;
	arr.fill(TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 2047));
	return arr;
}

std::array<u16, 16> MinusOne(u16 const* i_pal)
{
	std::array<u16, 16> newPal;
	for (u8 i = 0; i < 16; ++i)
	{
		u8 const r = (i_pal[i] & VDPPALETTE_REDMASK) >> VDPPALETTE_REDSFT;
		u8 const g = (i_pal[i] & VDPPALETTE_GREENMASK) >> VDPPALETTE_GREENSFT;
		u8 const b = (i_pal[i] & VDPPALETTE_BLUEMASK) >> VDPPALETTE_BLUESFT;
		newPal[i] = RGB3_3_3_TO_VDPCOLOR(
			(r > 0) ? r - 1 : 0,
			(g > 0) ? g - 1 : 0,
			(b > 0) ? b - 1 : 0
		);
	}

	return newPal;
}

std::array<u16, 16> Halve(u16 const* i_pal)
{
	std::array<u16, 16> newPal;
	for(u8 i = 0; i < 16; ++i)
	{
		newPal[i] = RGB3_3_3_TO_VDPCOLOR(
			(i_pal[i] & VDPPALETTE_REDMASK) >> (VDPPALETTE_REDSFT + 1),
			(i_pal[i] & VDPPALETTE_GREENMASK) >> (VDPPALETTE_GREENSFT + 1),
			(i_pal[i] & VDPPALETTE_BLUEMASK) >> (VDPPALETTE_BLUESFT + 1)
		);
	}

	return newPal;
}

constexpr std::array<u16, 64 * 32> c_hilightEmptyPlaneA = FillHilightPlaneA();

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

void HInt_TextArea_Restart()
{
	VDP_setHInterrupt(false);
}

void HInt_TextArea_SetName()
{
	VDP_setHIntCounter((c_textFramePos * 8 - 20) - 2 - 1);
	SetBGTextFramePalette(s_bgNamePal);
	SetCharTextFramePalette(s_charaNamePal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, false, c_textFramePos * 8 - 20, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	VDP_setHIntCounter((c_textFramePos * 8 + 0) - 2 - 1);
	SetBGTextFramePalette(s_bgTextPal);
	SetCharTextFramePalette(s_charaTextPal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 + 0, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	VDP_setHIntCounter(((c_textFramePos + c_textFrameHeight) * 8) - 2 - 1);
	SetBGTextFramePalette(s_bgNormalPal);
	SetCharTextFramePalette(s_charaNormalPal);

	// TODO: fix this more robustly. If the Hint at the end is interrupted by Vint, the game basically falls over because of all the DMA going on
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8, &HInt_TextArea_Restart>);
	//SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 + 8, &HInt_TextArea_SetName>);
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
	//if (!VDP_loadTileSet(image->tileset, basetile & TILE_INDEX_MASK, DMA_QUEUE))
	{
	//	return false;
	}

	u16 const tileChunks = image->tileset->numTile / 32;
	u16 const tileIndex = basetile & TILE_INDEX_MASK;
	for(u16 i = 0; i < tileChunks; ++i)
	{
		u16 const numTiles = i << 5;
		DMA_queueDma(DMA_VRAM, (void*)(image->tileset->tiles + (i << 8)), (tileIndex + numTiles) * 32, 32 * 16, 2);
	}
	u16 const remainder = image->tileset->numTile - (tileChunks * 32);
	if (remainder > 0)
	{
		u16 const numTiles = tileChunks << 5;
		DMA_queueDma(DMA_VRAM, (void*)(image->tileset->tiles + (tileChunks << 8)), (tileIndex + numTiles) * 32, remainder * 16, 2);
	}

	if (!VDP_setTileMapEx(plane, image->tilemap, basetile, x, y, 0, 0, image->tilemap->w, image->tilemap->h, DMA_QUEUE))
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
	// Way low. It'll take several frames but we'll cope
	DMA_setMaxTransferSize(512);

	// Enable shadow effects on text
	VDP_setHilightShadow(1);

	// Show palette-based text frame
	HInt_TextArea_SetName();

	m_printer.Init(io_game, vn_font, name_font);

	u16 blackWithTextPal[64] = { 0 };
	std::memcpy(blackWithTextPal + 48, text_font_pal.data, 16 * sizeof(u16));
	PAL_setColors(0, blackWithTextPal, 64, DMA_QUEUE_COPY);

	// Fill with reserved but highlighted empty tile
	VDP_setTileMapData(VDP_BG_A, c_hilightEmptyPlaneA.data(), 0, c_hilightEmptyPlaneA.size(), 2, DMA_QUEUE);

	// Wait a frame for colours to swap and tilemap to fill
	co_yield {};

	m_script->Init(io_game, *this, m_characters);

	VDP_setHInterrupt(true);

	io_game.AddVBlankCallback(
		[]
		{
			HInt_TextArea_SetName();
			VDP_setHInterrupt(true);
		}
	);

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

				m_bgNameCalcPal = Halve(s_bgNormalPal);
				s_bgNamePal = m_bgNameCalcPal.data();

				m_bgTextCalcPal = MinusOne(s_bgNamePal);
				s_bgTextPal = m_bgTextCalcPal.data();

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
				FastImageLoad(BG_A, m_nextPose->m_image, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - m_nextPose->m_image->tileset->numTile), 0, 0);
				//PAL_fadeInPalette(PAL1, m_nextPose->m_image->palette->data, FramesPerSecond() / 4, true);
				//PAL_setColors(PAL1 * 16, m_nextPose->m_image->palette->data, 16, DMA);
				s_charaNormalPal = m_nextPose->m_image->palette->data;

				m_charaNameCalcPal = Halve(s_charaNormalPal);
				s_charaNamePal = m_charaNameCalcPal.data();

				m_charaTextCalcPal = MinusOne(s_charaNamePal);
				s_charaTextPal = m_charaTextCalcPal.data();
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
	StopMusic(0);

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
	XGM_startPlay(i_bgm);
	XGM_setLoopNumber(i_loop ? -1 : 0);
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
		HideCharacter();
		m_nextPose = pose;
		//VDP_setHInterrupt(false);
		//PAL_fadeOutPalette(PAL1, FramesPerSecond() / 4, true);
	}
}

//------------------------------------------------------------------------------
void VNWorld::HideCharacter()
{
	// Fill with reserved but highlighted empty tile
	VDP_setTileMapData(VDP_BG_A, c_hilightEmptyPlaneA.data(), 0, c_hilightEmptyPlaneA.size(), 2, DMA_QUEUE);
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