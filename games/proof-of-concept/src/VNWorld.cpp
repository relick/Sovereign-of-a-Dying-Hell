#include "VNWorld.hpp"
#include "Game.hpp"
#include "Version.hpp"
#include "PaletteOps.hpp"
#include "Script.hpp"
#include "FadeOps.hpp"

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

void MinusOne(u16* i_dstPal, u16 const* i_srcPal)
{
	for (u8 i = 0; i < 16; ++i)
	{
		u8 const r = (i_srcPal[i] & VDPPALETTE_REDMASK) >> VDPPALETTE_REDSFT;
		u8 const g = (i_srcPal[i] & VDPPALETTE_GREENMASK) >> VDPPALETTE_GREENSFT;
		u8 const b = (i_srcPal[i] & VDPPALETTE_BLUEMASK) >> VDPPALETTE_BLUESFT;
		i_dstPal[i] = RGB3_3_3_TO_VDPCOLOR(
			(r > 0) ? r - 1 : 0,
			(g > 0) ? g - 1 : 0,
			(b > 0) ? b - 1 : 0
		);
	}
}

void Halve(u16* i_dstPal, u16 const* i_srcPal)
{
	for(u8 i = 0; i < 16; ++i)
	{
		i_dstPal[i] = RGB3_3_3_TO_VDPCOLOR(
			(i_srcPal[i] & VDPPALETTE_REDMASK) >> (VDPPALETTE_REDSFT + 1),
			(i_srcPal[i] & VDPPALETTE_GREENMASK) >> (VDPPALETTE_GREENSFT + 1),
			(i_srcPal[i] & VDPPALETTE_BLUEMASK) >> (VDPPALETTE_BLUESFT + 1)
		);
	}
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
Task SetTileMap_Full(u16 planeAddr, u16 const* tilemap, u16 w, u16 h, u16 basetile)
{
	//AutoProfileScope profile("SetTileMap_Full: %lu");

	u16 const* src = (u16 const*)FAR_SAFE(tilemap, mulu(w, h) * 2);

	// we can increment both index and palette
	u16 const baseinc = basetile & (TILE_INDEX_MASK | TILE_ATTR_PALETTE_MASK);
	// we can only do logical OR on priority and HV flip
	u16 const baseor = basetile & (TILE_ATTR_PRIORITY_MASK | TILE_ATTR_VFLIP_MASK | TILE_ATTR_HFLIP_MASK);

	u16 i = h;

	// get temp buffer and schedule DMA
	u16 const bufSize = mulu(planeWidth, h);

	u16* buf = nullptr;
	while (!(buf = static_cast<u16*>(DMA_allocateAndQueueDma(DMA_VRAM, planeAddr, bufSize, 2))))
	{
		co_yield{};
	}

	// Disable ints whilst we fill the DMA buffer
	//SYS_disableInts();
	u16 const bufInc = (planeWidth - w);

	u16 const quarterWidth = w >> 2;
	u16 const doubleWidth = w << 1;

	while (i--)
	{
		if (basetile != 0)
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

			r = w & 3;
			// prepare map data for row update
			while (r--) *buf++ = baseor | (*src++ + baseinc);

			buf += bufInc;
		}
		else
		{
			std::memcpy(buf, src, doubleWidth);
			src += w;
			buf += planeWidth;
		}
	}

	//SYS_enableInts();

	co_return;
}

template<bool t_Down, u16 t_Speed = 4>
Task SetTileMap_Wipe(u16 planeAddr, u16 const* tilemap, u16 w, u16 h, u16 basetile)
{
	//AutoProfileScope profile("SetTileMap_WipeDown: %lu");

	u16 const* src = (u16 const*)FAR_SAFE(tilemap, mulu(w, h) * 2);

	// we can increment both index and palette
	u16 const baseinc = basetile & (TILE_INDEX_MASK | TILE_ATTR_PALETTE_MASK);
	// we can only do logical OR on priority and HV flip
	u16 const baseor = basetile & (TILE_ATTR_PRIORITY_MASK | TILE_ATTR_VFLIP_MASK | TILE_ATTR_HFLIP_MASK);

	u16 i = h;

	// Disable ints whilst we fill the DMA buffer
	//SYS_disableInts();

	u16 const quarterWidth = w >> 2;
	u16 const doubleWidth = w << 1;

	u16 rowsDone = 0;

	if constexpr (t_Down)
	{
		u16 addr = planeAddr;
		u16 const addrInc = planeWidth * 2;

		while (i--)
		{
			// get temp buffer and schedule DMA
			u16* buf = nullptr;
			while (!(buf = static_cast<u16*>(DMA_allocateAndQueueDma(DMA_VRAM, addr, w, 2))))
			{
				co_yield{};
			}

			if (basetile != 0)
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

				r = w & 3;
				// prepare map data for row update
				while (r--) *buf++ = baseor | (*src++ + baseinc);
			}
			else
			{
				std::memcpy(buf, src, doubleWidth);
				src += w;
			}

			addr += addrInc;

			++rowsDone;
			if (rowsDone >= t_Speed)
			{
				rowsDone = 0;
				co_yield{};
			}
		}
	}
	else
	{
		u16 addr = planeAddr + (planeWidth * (h - 1) * 2);
		u16 const addrDec = planeWidth * 2;

		src += mulu(w, h - 1);
		u16 const srcDec = w * 2;

		while (i--)
		{
			// get temp buffer and schedule DMA
			u16* buf = nullptr;
			while (!(buf = static_cast<u16*>(DMA_allocateAndQueueDma(DMA_VRAM, addr, w, 2))))
			{
				co_yield{};
			}

			if (basetile != 0)
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

				r = w & 3;
				// prepare map data for row update
				while (r--) *buf++ = baseor | (*src++ + baseinc);

				src -= srcDec;
			}
			else
			{
				std::memcpy(buf, src, doubleWidth);
				src -= w;
			}

			addr -= addrDec;

			++rowsDone;
			if (rowsDone >= t_Speed)
			{
				rowsDone = 0;
				co_yield{};
			}
		}
	}

	//SYS_enableInts();

	co_return;
}

// Based on VDP_drawImageEx in part
Task FastTilesLoad(const Image* image, u16 basetile)
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
	s_bgNormalPal = m_mainPals.data();
	s_bgNamePal = m_namePals.data();
	s_bgTextPal = m_textPals.data();
	s_charaNormalPal = m_mainPals.data() + 16;
	s_charaNamePal = m_namePals.data() + 16;
	s_charaTextPal = m_textPals.data() + 16;
	HInt_TextArea_SetName();
	VDP_setHIntCounter(1);
	VDP_setHInterrupt(true);

	m_printer.Init(io_game, vn_font, name_font);

	u16 blackWithTextPal[64] = { 0 };
	std::memcpy(blackWithTextPal + 48, text_font_pal.data, 16 * sizeof(u16));
	PAL_setColors(0, blackWithTextPal, 64, DMA_QUEUE_COPY);

	HideCharacter(io_game, true);

	// Wait a frame for colours to swap and tilemap to fill
	co_yield{};
	while (io_game.TasksInProgress())
	{
		co_yield{};
	}

	m_script->Init(io_game, *this, m_characters);

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
	Game& io_game,
	Image const& i_bg
)
{
	m_nextBG = &i_bg;
	io_game.QueueLambdaTask([this] -> Task {
		{
			System::FadeOp fadeOp1 = System::CreateFade(m_mainPals.data(), palette_black, 16, FramesPerSecond() >> 1);
			System::FadeOp fadeOp2 = System::CreateFade(m_namePals.data(), palette_black, 16, FramesPerSecond() >> 1);
			System::FadeOp fadeOp3 = System::CreateFade(m_textPals.data(), palette_black, 16, FramesPerSecond() >> 1);

			while (fadeOp1)
			{
				fadeOp1.DoFadeStep();
				fadeOp2.DoFadeStep();
				fadeOp3.DoFadeStep();
				co_yield{};
			}
		}

		co_return;
	});
	io_game.QueueFunctionTask(FastTilesLoad(m_nextBG, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0)));
	io_game.QueueFunctionTask(SetTileMap_Full(VDP_BG_B, m_nextBG->tilemap->tilemap, m_nextBG->tilemap->w, m_nextBG->tilemap->h, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0)));
	io_game.QueueLambdaTask([this]() -> Task {
		m_bgSrcPal = m_nextBG->palette->data;

		{
			std::array<u16, 16> namePal;
			std::array<u16, 16> textPal;

			Halve(namePal.data(), m_bgSrcPal);
			MinusOne(textPal.data(), namePal.data());

			System::FadeOp fadeOp1 = System::CreateFade(m_mainPals.data(), m_bgSrcPal, 16, FramesPerSecond() >> 1);
			System::FadeOp fadeOp2 = System::CreateFade(m_namePals.data(), namePal.data(), 16, FramesPerSecond() >> 1);
			System::FadeOp fadeOp3 = System::CreateFade(m_textPals.data(), textPal.data(), 16, FramesPerSecond() >> 1);

			while (fadeOp1)
			{
				fadeOp1.DoFadeStep();
				fadeOp2.DoFadeStep();
				fadeOp3.DoFadeStep();
				co_yield{};
			}
		}

		m_nextBG = nullptr;
		co_return;
	});
}

//------------------------------------------------------------------------------
void VNWorld::BlackBG
(
	Game& io_game
)
{
	io_game.QueueLambdaTask([this] -> Task {
		System::FadeOp fadeOp1 = System::CreateFade(m_mainPals.data(), palette_black, 16, FramesPerSecond() >> 2);
		System::FadeOp fadeOp2 = System::CreateFade(m_namePals.data(), palette_black, 16, FramesPerSecond() >> 2);
		System::FadeOp fadeOp3 = System::CreateFade(m_textPals.data(), palette_black, 16, FramesPerSecond() >> 2);

		while (fadeOp1)
		{
			fadeOp1.DoFadeStep();
			fadeOp2.DoFadeStep();
			fadeOp3.DoFadeStep();
			co_yield{};
		}

		co_return;
	});
}

//------------------------------------------------------------------------------
void VNWorld::SetCharacter
(
	Game& io_game,
	CharacterID i_charID,
	PoseID i_poseID
)
{
	auto const [_, pose] = m_characters.GetPose(i_charID, i_poseID);

	if (pose)
	{
		HideCharacter(io_game, false);
		m_nextPose = pose;

		io_game.QueueLambdaTask([this] -> Task {
			m_charaSrcPal = m_nextPose->m_image->palette->data;

			std::memcpy(m_mainPals.data() + 16, m_charaSrcPal, sizeof(u16) * 16);
			Halve(m_namePals.data() + 16, m_mainPals.data() + 16);
			MinusOne(m_textPals.data() + 16, m_namePals.data() + 16);

			co_return;
		});
		io_game.QueueFunctionTask(FastTilesLoad(m_nextPose->m_image, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - m_nextPose->m_image->tileset->numTile)));
		io_game.QueueFunctionTask(SetTileMap_Wipe<false>(VDP_BG_A, m_nextPose->m_image->tilemap->tilemap, m_nextPose->m_image->tilemap->w, m_nextPose->m_image->tilemap->h, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1536 - m_nextPose->m_image->tileset->numTile)));
		io_game.QueueLambdaTask([this] -> Task {
			m_nextPose = nullptr;
			co_return;
		});
	}
}

//------------------------------------------------------------------------------
void VNWorld::HideCharacter
(
	Game& io_game,
	bool i_fast
)
{
	// Fill with reserved but highlighted empty tile
	if (i_fast)
	{
		io_game.QueueFunctionTask([] -> Task {
			//AutoProfileScope profile("VNWorld::HideCharacter: %lu");
			while (!DMA_transfer(DMA_QUEUE, DMA_VRAM, (void*)c_hilightEmptyPlaneA.data(), VDP_BG_A, c_hilightEmptyPlaneA.size(), 2))
			{
				co_yield {};
			}

			co_return;
		}());
	}
	else
	{
		io_game.QueueFunctionTask(SetTileMap_Wipe<true>(VDP_BG_A, c_hilightEmptyPlaneA.data(), 40, 28, 0));
	}
}

//------------------------------------------------------------------------------
void VNWorld::SetCharName
(
	CharacterID i_charID
)
{
	auto const chara = m_characters.GetCharacter(i_charID);
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