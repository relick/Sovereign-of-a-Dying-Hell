#include "VNWorld.hpp"

#include "CharacterData.hpp"
#include "Debug.hpp"
#include "FadeOps.hpp"
#include "Game.hpp"
#include "PaletteOps.hpp"
#include "Script.hpp"
#include "TileOps.hpp"
#include "Version.hpp"

#include "res_fonts.h"

#include <algorithm>
#include <cstring>

namespace Game
{

consteval std::array<u16, 64*32> FillHighlightPlaneA()
{
	std::array<u16, 64 * 32> arr;
	arr.fill(TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 2047));
	return arr;
}

constexpr std::array<u16, 64 * 32> c_highlightEmptyPlaneA = FillHighlightPlaneA();
constexpr Palettes::RGB3Colour c_tintColour{ 0, 1, 2 }; // TODO: customisable by player/scene/character?

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
	Palettes::SetBGTextFramePalette(s_bgNamePal);
	Palettes::SetCharTextFramePalette(s_charaNamePal);
	SYS_setHIntCallback(&Palettes::HInt_TextFrameDMA2<PAL0, PAL1, false, c_textFramePos * 8 - 21, &HInt_TextArea_SetText>);
}

void HInt_TextArea_SetText()
{
	Palettes::SetBGTextFramePalette(s_bgTextPal);
	Palettes::SetCharTextFramePalette(s_charaTextPal);
	SYS_setHIntCallback(&Palettes::HInt_TextFrameDMA2<PAL0, PAL1, true, c_textFramePos * 8 - 1, &HInt_TextArea_Reset>);
}

void HInt_TextArea_Reset()
{
	Palettes::SetBGTextFramePalette(s_bgNormalPal);
	Palettes::SetCharTextFramePalette(s_charaNormalPal);

	// TODO: fix this more robustly. If the Hint at the end is interrupted by Vint, the game basically falls over because of all the DMA going on
	SYS_setHIntCallback(&Palettes::HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8 + 1, &HInt_TextArea_SetName>);
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

	m_fonts.Init(vn_font, name_font);

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

	std::array<u16, 64> blackWithTextPal = { 0 };
	std::copy(text_font_pal.data, text_font_pal.data + 16, blackWithTextPal.begin() + 48);
	PAL_setColors(0, blackWithTextPal.data(), 64, DMA_QUEUE_COPY);

	HideCharacterVisual(io_game, true);

	// Wait a frame for colours to swap and tilemap to fill
	co_yield{};
	while (io_game.TasksInProgress())
	{
		co_yield{};
	}

	m_script->Init(io_game, *this);

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
	
	co_return;
}

//------------------------------------------------------------------------------
void VNWorld::Run
(
	Game& io_game
)
{
	if (m_waitingForTasksStack > 0)
	{
		return;
	}

	m_animator.Update(io_game);

	u16 const buttons = JOY_readJoypad(JOY_1);
	bool const ABCpressedThisFrame = [this, buttons]{
		if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
		{
			if (!m_ABCpressed)
			{
				m_ABCpressed = true;
				return true;
			}
		}
		else
		{
			m_ABCpressed = false;
		}
		return false;
	}();

	switch (static_cast<SceneMode>(m_sceneMode.index()))
	{
	case SceneMode::None:
	{
		break;
	}
	case SceneMode::Dialogue:
	{
		if (ABCpressedThisFrame)
		{
			Get<SceneMode::Dialogue>().Next();
		}
		else
		{
			Get<SceneMode::Dialogue>().Update();
		}
		break;
	}
	case SceneMode::Choice:
	{
		// nyi
		break;
	}
	case SceneMode::Settings:
	{
		// nyi
		break;
	}
	}

	{
		//AutoProfileScope profile("BuryYourGays_Script::Update: %lu");
		switch (m_progressMode)
		{
		case ProgressMode::Always:
		{
			m_script->Update(io_game, *this);
			break;
		}
		case ProgressMode::Dialogue:
		{
			if (ABCpressedThisFrame && Get<SceneMode::Dialogue>().Done())
			{
				m_progressMode = ProgressMode::Always;
				m_script->Update(io_game, *this);
			}
			break;
		}
		case ProgressMode::Choice:
		{
			// nyi
			break;
		}
		}
	}
}

//------------------------------------------------------------------------------
void VNWorld::WaitForTasks
(
	Game& io_game
)
{
	++m_waitingForTasksStack;
	io_game.QueueLambdaTask([this] -> Task {
		--m_waitingForTasksStack;
		co_return;
	});
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
			Palettes::FadeOp fadeOp1 = Palettes::CreateFade(m_mainPals.data(), palette_black, 16, FramesPerSecond() >> 1);
			Palettes::FadeOp fadeOp2 = Palettes::CreateFade(m_namePals.data(), palette_black, 16, FramesPerSecond() >> 1);
			Palettes::FadeOp fadeOp3 = Palettes::CreateFade(m_textPals.data(), palette_black, 16, FramesPerSecond() >> 1);

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
	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(
		m_nextBG->tileset,
		0
	));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(
		VDP_BG_B,
		m_nextBG->tilemap->tilemap,
		m_nextBG->tilemap->w,
		m_nextBG->tilemap->h,
		TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 0)
	));
	io_game.QueueLambdaTask([this]() -> Task {
		m_bgSrcPal = m_nextBG->palette->data;

		{
			std::array<u16, 16> namePal;
			std::array<u16, 16> textPal;

			Palettes::Tint(namePal.data(), m_bgSrcPal, c_tintColour);
			Palettes::MinusOne(textPal.data(), namePal.data());

			Palettes::FadeOp fadeOp1 = Palettes::CreateFade(m_mainPals.data(), m_bgSrcPal, 16, FramesPerSecond() >> 1);
			Palettes::FadeOp fadeOp2 = Palettes::CreateFade(m_namePals.data(), namePal.data(), 16, FramesPerSecond() >> 1);
			Palettes::FadeOp fadeOp3 = Palettes::CreateFade(m_textPals.data(), textPal.data(), 16, FramesPerSecond() >> 1);

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
		Palettes::FadeOp fadeOp1 = Palettes::CreateFade(m_mainPals.data(), palette_black, 16, FramesPerSecond() >> 2);
		Palettes::FadeOp fadeOp2 = Palettes::CreateFade(m_namePals.data(), palette_black, 16, FramesPerSecond() >> 2);
		Palettes::FadeOp fadeOp3 = Palettes::CreateFade(m_textPals.data(), palette_black, 16, FramesPerSecond() >> 2);

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
void VNWorld::SetCharacterVisual
(
	Game& io_game,
	Pose const& i_pose
)
{
	HideCharacterVisual(io_game, false);
	m_nextPose = &i_pose;

	io_game.QueueLambdaTask([this] -> Task {
		m_charaSrcPal = m_nextPose->m_palette->data;

		std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_mainPals.begin() + 16);
		Palettes::Tint(m_namePals.data() + 16, m_mainPals.data() + 16, c_tintColour);
		Palettes::MinusOne(m_textPals.data() + 16, m_namePals.data() + 16);

		co_return;
	});
	u16 const tileIndex = 1536 - m_nextPose->m_tileset->numTile;
	u16 const baseTile = TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, tileIndex);
	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(
		m_nextPose->m_tileset,
		tileIndex
	));
	io_game.QueueFunctionTask(Tiles::SetMap_Wipe<Tiles::WipeDir::Up>(
		VDP_BG_A,
		m_nextPose->m_animation[0].m_tilemap->tilemap,
		m_nextPose->m_animation[0].m_tilemap->w,
		m_nextPose->m_animation[0].m_tilemap->h,
		baseTile
	));
	io_game.QueueLambdaTask([this, baseTile] -> Task {
		m_animator.StartAnimation(m_nextPose, baseTile);
		m_nextPose = nullptr;
		co_return;
	});
}

//------------------------------------------------------------------------------
void VNWorld::HideCharacterVisual
(
	Game& io_game,
	bool i_fast
)
{
	m_animator.StopAnimation();

	// Fill with reserved but highlighted empty tile
	if (i_fast)
	{
		io_game.QueueFunctionTask([] -> Task {
			//AutoProfileScope profile("VNWorld::HideCharacter: %lu");
			while (!DMA_transfer(DMA_QUEUE, DMA_VRAM, (void*)c_highlightEmptyPlaneA.data(), VDP_BG_A, c_highlightEmptyPlaneA.size(), 2))
			{
				co_yield {};
			}

			co_return;
		}());
	}
	else
	{
		io_game.QueueFunctionTask(Tiles::SetMap_Wipe<Tiles::WipeDir::Down>(
			VDP_BG_A,
			c_highlightEmptyPlaneA.data(),
			40,
			28,
			0
		));
	}
}

//------------------------------------------------------------------------------
void VNWorld::SetText
(
	Game& io_game,
	Character const* i_char,
	char const* i_text
)
{
	TransitionTo(io_game, SceneMode::Dialogue);
	m_progressMode = ProgressMode::Dialogue;
	
	if (i_char)
	{
		Get<SceneMode::Dialogue>().SetName(i_char->m_displayName, i_char->m_showOnLeft);
	}
	else
	{
		Get<SceneMode::Dialogue>().SetName(nullptr, true);
	}

	Get<SceneMode::Dialogue>().SetText(i_text);
}

//------------------------------------------------------------------------------
void VNWorld::Choice
(
	Game& io_game,
	std::span<char const* const> i_choices
)
{
	TransitionTo(io_game, SceneMode::Choice);
	m_progressMode = ProgressMode::Choice;

	Get<SceneMode::Choice>().SetChoices(i_choices);
}

//------------------------------------------------------------------------------
void VNWorld::TransitionTo
(
	Game& io_game,
	SceneMode i_sceneMode
)
{
	if (static_cast<u8>(i_sceneMode) == m_sceneMode.index())
	{
		return;
	}

	switch (i_sceneMode)
	{
	case SceneMode::None:
	{
		m_sceneMode.emplace<static_cast<u8>(SceneMode::None)>();
		break;
	}
	case SceneMode::Dialogue:
	{
		m_sceneMode.emplace<static_cast<u8>(SceneMode::Dialogue)>(io_game, m_fonts);
		break;
	}
	case SceneMode::Choice:
	{
		m_sceneMode.emplace<static_cast<u8>(SceneMode::Choice)>(io_game, m_fonts);
		break;
	}
	case SceneMode::Settings:
	{
		m_sceneMode.emplace<static_cast<u8>(SceneMode::Settings)>();
		break;
	}
	}
}

}