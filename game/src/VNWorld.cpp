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

constexpr Palettes::RGB3Colour c_tintColour{ 0, 0, 0 }; // TODO: customisable by player/scene/character?
constexpr u8 c_inputBufferingTimer = 30; // Number of frames an input can be buffered before being discarded if unprocessed


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
	std::unique_ptr<Script> i_script
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

	m_script->InitVN(io_game, *this);

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

	if (m_ABCbufferTimer > 0)
	{
		--m_ABCbufferTimer;
		if (m_ABCbufferTimer == 0)
		{
			m_ABCbuffered = 0;
		}
	}
	if (ABCpressedThisFrame)
	{
		m_ABCbufferTimer = c_inputBufferingTimer;
		++m_ABCbuffered;
	}

	bool choiceOccurred = false;

	switch (CurrentMode())
	{
	case SceneMode::None:
	{
		break;
	}
	case SceneMode::Dialogue:
	{
		if (!Get<SceneMode::Dialogue>().Done() && m_ABCbuffered > 0)
		{
			--m_ABCbuffered;
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
		auto const updateResult = Get<SceneMode::Choice>().Update(m_ABCbuffered > 0);
		if (updateResult)
		{
			--m_ABCbuffered;
			m_choiceMade = updateResult.value();
			choiceOccurred = true;
		}
		else if (updateResult.error() == ChoiceSystem::NoChoiceMade::TimeLimitReached)
		{
			m_choiceMade = std::nullopt;
			choiceOccurred = true;
		}
		// Otherwise, still waiting
		break;
	}
	case SceneMode::Settings:
	{
		// nyi
		break;
	}
	}

	// TODO
	// This is a bug fix but risks making UI unresponsive
	// Essentially, prevent the script progressing until we're clear of tasks, in order to prevent
	// bugs of transitioning scene mode before tasks are done.
	// There will be many frames where we're not clear due to animations and sprite changes etc.
	// So it may be necessary to do some input buffering to improve responsiveness
	if (io_game.TasksInProgress())
	{
		return;
	}

	{
		//AutoProfileScope profile("BuryYourGays_Script::Update: %lu");
		switch (m_progressMode)
		{
		case ProgressMode::Always:
		{
			m_script->UpdateVN(io_game, *this);
			break;
		}
		case ProgressMode::Dialogue:
		{
			if (m_ABCbuffered > 0 && Get<SceneMode::Dialogue>().Done())
			{
				--m_ABCbuffered;
				m_progressMode = ProgressMode::Always;
				m_script->UpdateVN(io_game, *this);
			}
			break;
		}
		case ProgressMode::Choice:
		{
			if (choiceOccurred)
			{
				// We have MADE OUR CHOICE!
				m_progressMode = ProgressMode::Always;
				m_script->UpdateVN(io_game, *this);
				m_choiceMade = std::nullopt;
			}
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
	io_game.QueueLambdaTask([this, &i_bg] -> Task {
		{
			Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data(), palette_black, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data(), palette_black, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data(), palette_black, FramesPerSecond() >> 1);

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
		i_bg.tileset,
		c_tilesStart
	));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(
		VDP_BG_B,
		i_bg.tilemap->tilemap,
		i_bg.tilemap->w,
		i_bg.tilemap->h,
		TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, c_tilesStart)
	));
	io_game.QueueLambdaTask([this, &i_bg]() -> Task {
		m_bgSrcPal = i_bg.palette->data;

		if (CurrentMode() == SceneMode::Dialogue)
		{
			std::array<u16, 16> namePal;
			std::array<u16, 16> textPal;

			Palettes::Tint(m_bgSrcPal, namePal.data(), c_tintColour);
			Palettes::MinusOne(namePal.data(), textPal.data());

			Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data(), namePal.data(), FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data(), textPal.data(), FramesPerSecond() >> 1);

			while (fadeOp1)
			{
				fadeOp1.DoFadeStep();
				fadeOp2.DoFadeStep();
				fadeOp3.DoFadeStep();
				co_yield{};
			}
		}
		else if (CurrentMode() == SceneMode::Choice)
		{
			std::array<u16, 16> darkPal;
			Palettes::Tint(m_bgSrcPal, darkPal.data(), c_tintColour);

			Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data(), darkPal.data(), FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data(), darkPal.data(), FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data(), darkPal.data(), FramesPerSecond() >> 1);

			while (fadeOp1)
			{
				fadeOp1.DoFadeStep();
				fadeOp2.DoFadeStep();
				fadeOp3.DoFadeStep();
				co_yield{};
			}
		}
		else
		{
			Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data(), m_bgSrcPal, FramesPerSecond() >> 1);

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
}

//------------------------------------------------------------------------------
void VNWorld::BlackBG
(
	Game& io_game
)
{
	io_game.QueueLambdaTask([this] -> Task {
		Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data(), palette_black, FramesPerSecond() >> 2);
		Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data(), palette_black, FramesPerSecond() >> 2);
		Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data(), palette_black, FramesPerSecond() >> 2);

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

	// One big mega task, since various things are run as conditional sub tasks
	io_game.QueueLambdaTask([this, &i_pose] -> Task {
		m_charaSrcPal = i_pose.m_palette->data;

		if (CurrentMode() == SceneMode::Dialogue)
		{
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_mainPals.begin() + 16);
			Palettes::Tint(m_mainPals.data() + 16, m_namePals.data() + 16, c_tintColour);
			Palettes::MinusOne(m_namePals.data() + 16, m_textPals.data() + 16);
		}
		else if (CurrentMode() == SceneMode::Choice)
		{
			Palettes::Tint(m_charaSrcPal, m_mainPals.data() + 16, c_tintColour);
			std::copy(m_mainPals.begin() + 16, m_mainPals.begin() + 32, m_namePals.begin() + 16);
			std::copy(m_mainPals.begin() + 16, m_mainPals.begin() + 32, m_textPals.begin() + 16);
		}
		else
		{
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_mainPals.begin() + 16);
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_namePals.begin() + 16);
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_textPals.begin() + 16);
		}

		u16 const tileIndex = c_tilesEnd - i_pose.m_tileset->numTile;
		u16 const baseTile = TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, tileIndex);

		if (m_existingEndTileSet != i_pose.m_tileset)
		{
			{
				Task load = Tiles::LoadTiles_Chunked(
					i_pose.m_tileset,
					tileIndex
				);
				AwaitTask(load);
			}

			co_yield{};

			{
				Task wipe = Tiles::SetMap_Wipe<Tiles::WipeDir::Up>(
					VDP_BG_A,
					i_pose.m_animation[0].m_tilemap->tilemap,
					i_pose.m_animation[0].m_tilemap->w,
					i_pose.m_animation[0].m_tilemap->h,
					baseTile
				);
				AwaitTask(wipe);
			}
		}

		m_existingEndTileSet = i_pose.m_tileset;

		m_animator.StartAnimation(i_pose, baseTile);

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
	io_game.QueueLambdaTask([this] -> Task {
		m_animator.StopAnimation();
		co_return;
	});

	// Fill with reserved but highlighted empty tile
	if (i_fast)
	{
		io_game.QueueFunctionTask(Tiles::ClearMap_Full(
			VDP_BG_A,
			Tiles::c_emptyPlane_Highlight
		));
	}
	else
	{
		io_game.QueueFunctionTask(Tiles::SetMap_Wipe<Tiles::WipeDir::Down>(
			VDP_BG_A,
			Tiles::c_emptyPlane_Highlight.data(),
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
	char const* i_text,
	std::optional<SFXID> i_beeps
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

	Get<SceneMode::Dialogue>().SetText(i_text, i_beeps);
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
void VNWorld::TimedChoice
(
	Game& io_game,
	std::span<char const* const> i_choices,
	f16 i_timeInSeconds
)
{
	TransitionTo(io_game, SceneMode::Choice);
	m_progressMode = ProgressMode::Choice;

	Get<SceneMode::Choice>().SetChoices(i_choices, i_timeInSeconds);
}

//------------------------------------------------------------------------------
void VNWorld::ClearMode
(
	Game& io_game
)
{
	TransitionTo(io_game, SceneMode::None);
}

//------------------------------------------------------------------------------
void VNWorld::TransitionTo
(
	Game& io_game,
	SceneMode i_sceneMode
)
{
	if (i_sceneMode == CurrentMode())
	{
		return;
	}

	switch (i_sceneMode)
	{
	case SceneMode::None:
	{
		io_game.QueueLambdaTask([this] -> Task {
			Palettes::FadeOp<16> fadeOp1a = Palettes::CreateFade<16>(m_mainPals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp1b = Palettes::CreateFade<16>(m_mainPals.data() + 16, m_charaSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp2a = Palettes::CreateFade<16>(m_namePals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp2b = Palettes::CreateFade<16>(m_namePals.data() + 16, m_charaSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp3a = Palettes::CreateFade<16>(m_textPals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp3b = Palettes::CreateFade<16>(m_textPals.data() + 16, m_charaSrcPal, FramesPerSecond() >> 1);

			while (fadeOp1a)
			{
				fadeOp1a.DoFadeStep();
				fadeOp1b.DoFadeStep();
				fadeOp2a.DoFadeStep();
				fadeOp2b.DoFadeStep();
				fadeOp3a.DoFadeStep();
				fadeOp3b.DoFadeStep();
				co_yield{};
			}
		});

		m_sceneMode.emplace<static_cast<u8>(SceneMode::None)>();
		break;
	}
	case SceneMode::Dialogue:
	{
		io_game.QueueLambdaTask([this] -> Task {
			std::array<u16, 32> namePal;
			std::array<u16, 32> textPal;

			Palettes::Tint<16>(m_bgSrcPal, namePal.data(), c_tintColour);
			Palettes::Tint<16>(m_charaSrcPal, namePal.data() + 16, c_tintColour);
			Palettes::MinusOne<16>(namePal.data(), textPal.data());
			Palettes::MinusOne<16>(namePal.data() + 16, textPal.data() + 16);

			Palettes::FadeOp<16> fadeOp1a = Palettes::CreateFade<16>(m_mainPals.data(), m_bgSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<16> fadeOp1b = Palettes::CreateFade<16>(m_mainPals.data() + 16, m_charaSrcPal, FramesPerSecond() >> 1);
			Palettes::FadeOp<32> fadeOp2 = Palettes::CreateFade<32>(m_namePals.data(), namePal.data(), FramesPerSecond() >> 1);
			Palettes::FadeOp<32> fadeOp3 = Palettes::CreateFade<32>(m_textPals.data(), textPal.data(), FramesPerSecond() >> 1);

			while (fadeOp1a)
			{
				fadeOp1a.DoFadeStep();
				fadeOp1b.DoFadeStep();
				fadeOp2.DoFadeStep();
				fadeOp3.DoFadeStep();
				co_yield{};
			}
		});

		m_sceneMode.emplace<static_cast<u8>(SceneMode::Dialogue)>(io_game, m_fonts);
		break;
	}
	case SceneMode::Choice:
	{
		io_game.QueueLambdaTask([this] -> Task {
			std::array<u16, 32> darkPal;

			Palettes::Tint<16>(m_bgSrcPal, darkPal.data(), c_tintColour);
			Palettes::Tint<16>(m_charaSrcPal, darkPal.data() + 16, c_tintColour);

			Palettes::FadeOp<32> fadeOp1 = Palettes::CreateFade<32>(m_mainPals.data(), darkPal.data(), FramesPerSecond() >> 1);
			Palettes::FadeOp<32> fadeOp2 = Palettes::CreateFade<32>(m_namePals.data(), darkPal.data(), FramesPerSecond() >> 1);
			Palettes::FadeOp<32> fadeOp3 = Palettes::CreateFade<32>(m_textPals.data(), darkPal.data(), FramesPerSecond() >> 1);

			while (fadeOp1)
			{
				fadeOp1.DoFadeStep();
				fadeOp2.DoFadeStep();
				fadeOp3.DoFadeStep();
				co_yield{};
			}
		});

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