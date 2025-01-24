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

void HInt_TextArea_SetName()
{
	SetBGTextFramePalette(s_bgNamePal);
	SetCharTextFramePalette(s_charaNamePal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, false, c_textFramePos * 8 - 21, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	SetBGTextFramePalette(s_bgTextPal);
	SetCharTextFramePalette(s_charaTextPal);
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 - 1, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	SetBGTextFramePalette(s_bgNormalPal);
	SetCharTextFramePalette(s_charaNormalPal);

	// TODO: fix this more robustly. If the Hint at the end is interrupted by Vint, the game basically falls over because of all the DMA going on
	SYS_setHIntCallback(&HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8 + 1, &HInt_TextArea_SetName>);
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

// Based on VDP_setTileMapEx
DMARoutine FastSetTileMap(u16 planeAddr, const TileMap* tilemap, u16 basetile)
{
	//AutoProfileScope profile("FastSetTileMap: %lu");

	u16 const* src = (u16 const*)FAR_SAFE(tilemap->tilemap, mulu(tilemap->w, tilemap->h) * 2);

	// we can increment both index and palette
	u16 const baseinc = basetile & (TILE_INDEX_MASK | TILE_ATTR_PALETTE_MASK);
	// we can only do logical OR on priority and HV flip
	u16 const baseor = basetile & (TILE_ATTR_PRIORITY_MASK | TILE_ATTR_VFLIP_MASK | TILE_ATTR_HFLIP_MASK);

	u16 i = tilemap->h;

	// get temp buffer and schedule DMA
	u16 const bufSize = mulu(planeWidth, tilemap->h);

	u16* buf = nullptr;
	while (!(buf = static_cast<u16*>(DMA_allocateAndQueueDma(DMA_VRAM, planeAddr, bufSize, 2))))
	{
		co_yield{};
	}

	// Disable ints whilst we fill the DMA buffer
	//SYS_disableInts();
	u16 const bufInc = (planeWidth - tilemap->w);

	u16 const quarterWidth = tilemap->w >> 2;

	while (i--)
	{
		// then prepare data in buffer that will be transferred by DMA
		u16 r = quarterWidth;

		// prepare map data for row update
		while (r--)
		{
			*buf++ = baseor | (*src++ + baseinc);
			*buf++ = baseor | (*src++ + baseinc);
			*buf++ = baseor | (*src++ + baseinc);
			*buf++ = baseor | (*src++ + baseinc);
		}

		r = tilemap->w & 3;
		// prepare map data for row update
		while (r--) *buf++ = baseor | (*src++ + baseinc);

		buf += bufInc;
	}

	//SYS_enableInts();

	co_return;
}

// Based on VDP_drawImageEx in part
DMARoutine FastTilesLoad(const Image* image, u16 basetile)
{
	//AutoProfileScope profile("FastTilesLoad: %lu");
	u16 constexpr chunkShift = 5;
	u16 constexpr chunkSize = 1 << chunkShift;

	u16 const tileChunks = image->tileset->numTile >> chunkShift;
	u16 const baseTileIndex = basetile & TILE_INDEX_MASK;
	u16 tileIndex = baseTileIndex << 5;
	u16 tileInc = 1 << (chunkShift + 5);
	u32 const* srcTiles = image->tileset->tiles;
	u16 srcTilesInc = 1 << (chunkShift + 3);
	for(u16 i = 0; i < tileChunks; ++i)
	{
		while (!DMA_queueDma(DMA_VRAM, (void*)srcTiles, tileIndex, chunkSize * 16, 2))
		{
			co_yield {};
		}
		srcTiles += srcTilesInc;
		tileIndex += tileInc;
	}
	u16 const remainder = image->tileset->numTile - (tileChunks << chunkShift);
	if (remainder > 0)
	{
		while (!DMA_queueDma(DMA_VRAM, (void*)srcTiles, tileIndex, remainder * 16, 2))
		{
			co_yield {};
		}
	}

	co_return;
}

//------------------------------------------------------------------------------
WorldRoutine VNWorld::Init
(
	Game& io_game
)
{
	// Way low. It'll take several frames but we'll cope
	DMA_setMaxTransferSize(4096);
	DMA_setIgnoreOverCapacity(true);

	// Enable shadow effects on text
	VDP_setHilightShadow(1);

	// Show palette-based text frame
	HInt_TextArea_SetName();
	VDP_setHIntCounter(1);

	m_printer.Init(io_game, vn_font, name_font);

	u16 blackWithTextPal[64] = { 0 };
	std::memcpy(blackWithTextPal + 48, text_font_pal.data, 16 * sizeof(u16));
	PAL_setColors(0, blackWithTextPal, 64, DMA_QUEUE_COPY);

	HideCharacter(io_game);

	// Wait a frame for colours to swap and tilemap to fill
	co_yield{};
	while (io_game.DMAsInProgress())
	{
		co_yield{};
	}

	m_script->Init(io_game, *this, m_characters);

	VDP_setHInterrupt(true);

	co_return;
}

//------------------------------------------------------------------------------
WorldRoutine VNWorld::Shutdown
(
	Game& io_game
)
{
	StopMusic(0);

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
	if (PAL_isDoingFade() || io_game.DMAsInProgress())
	{
		return;
	}

	if (m_nextBG)
	{
		io_game.AddDMARoutine(FastTilesLoad(m_nextBG, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0)));
		io_game.AddDMARoutine(FastSetTileMap(VDP_BG_B, m_nextBG->tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0)));
		io_game.AddDMARoutine([this] -> DMARoutine { PAL_fadeInPalette(PAL0, m_nextBG->palette->data, FramesPerSecond() / 4, true); co_return; }());

		s_bgNormalPal = m_nextBG->palette->data;

		m_bgNameCalcPal = Halve(s_bgNormalPal);
		s_bgNamePal = m_bgNameCalcPal.data();

		m_bgTextCalcPal = MinusOne(s_bgNamePal);
		s_bgTextPal = m_bgTextCalcPal.data();

		m_nextBG = nullptr;
		return;
	}

	if (m_nextPose)
	{
		//PAL_fadeInPalette(PAL1, m_nextPose->m_image->palette->data, FramesPerSecond() / 4, true);
		//PAL_setColors(PAL1 * 16, m_nextPose->m_image->palette->data, 16, DMA);
		m_nextPose = nullptr;
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
				//AutoProfileScope profile("BuryYourGays_Script::Update: %lu");

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
	Game& io_game,
	char const* i_charName,
	char const* i_poseName
)
{
	auto const [_, pose] = m_characters.FindPose(i_charName, i_poseName);
	if (pose)
	{
		HideCharacter(io_game);
		m_nextPose = pose;
		io_game.AddDMARoutine([this] -> DMARoutine {
			s_charaNormalPal = m_nextPose->m_image->palette->data;

			m_charaNameCalcPal = Halve(s_charaNormalPal);
			s_charaNamePal = m_charaNameCalcPal.data();

			m_charaTextCalcPal = MinusOne(s_charaNamePal);
			s_charaTextPal = m_charaTextCalcPal.data();

			co_return;
		}());
		io_game.AddDMARoutine(FastTilesLoad(m_nextPose->m_image, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - m_nextPose->m_image->tileset->numTile)));
		io_game.AddDMARoutine(FastSetTileMap(VDP_BG_A, m_nextPose->m_image->tilemap, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - m_nextPose->m_image->tileset->numTile)));
		//VDP_setHInterrupt(false);
		//PAL_fadeOutPalette(PAL1, FramesPerSecond() / 4, true);
	}
}

//------------------------------------------------------------------------------
void VNWorld::HideCharacter
(
	Game& io_game
)
{
	// Fill with reserved but highlighted empty tile
	io_game.AddDMARoutine([] -> DMARoutine {
		//AutoProfileScope profile("VNWorld::HideCharacter: %lu");
		while (!DMA_transfer(DMA_QUEUE, DMA_VRAM, (void*)c_hilightEmptyPlaneA.data(), VDP_BG_A, c_hilightEmptyPlaneA.size(), 2))
		{
			co_yield {};
		}

		co_return;
	}());
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