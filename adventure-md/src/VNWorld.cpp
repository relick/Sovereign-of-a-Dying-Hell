#include "adventure-md/VNWorld.hpp"

#include "adventure-md/CharacterData.hpp"
#include "adventure-md/Debug.hpp"
#include "adventure-md/FadeOps.hpp"
#include "adventure-md/Game.hpp"
#include "adventure-md/PaletteOps.hpp"
#include "adventure-md/Script.hpp"
#include "adventure-md/TileOps.hpp"
#include "adventure-md/Version.hpp"

#include "res/fonts.h"

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

void HInt_TextArea_Reset_Only()
{
	Palettes::SetBGTextFramePalette(s_bgNormalPal);
	Palettes::SetCharTextFramePalette(s_charaNormalPal);

	SYS_setHIntCallback(&Palettes::HInt_TextFrameDMA2<PAL0, PAL1, true, (c_textFramePos + c_textFrameHeight) * 8 + 1, &HInt_TextArea_Reset_Only>);
}

//------------------------------------------------------------------------------
VNWorld::VNWorld
(
	std::unique_ptr<Script> i_script
)
	: m_script{ std::move(i_script) }
{
	Assert(m_script, "Must provide a script to VNWorld");
}

//------------------------------------------------------------------------------
WorldRoutine VNWorld::Init
(
	Game& io_game
)
{
	// Enable shadow effects on text
	VDP_setHilightShadow(1);

	m_fonts.Init(vn_font, vn_desc_font, name_font);

	// Show palette-based text frame
	s_bgNormalPal = m_mainPals.data();
	s_bgNamePal = m_namePals.data();
	s_bgTextPal = m_textPals.data();
	s_charaNormalPal = m_mainPals.data() + 16;
	s_charaNamePal = m_namePals.data() + 16;
	s_charaTextPal = m_textPals.data() + 16;
	HInt_TextArea_Reset_Only();
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

	// Create portrait sprite
	{
		m_portraitSprite = io_game.Sprites().AddInvisibleSprite(SpriteSize::r4c4, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, c_reservedPortraitTileStart));
		// Position 10x10 in from corner
		m_portraitSprite.SetX(10);
		m_portraitSprite.SetY(c_screenHeightPx - c_portraitSizePx - 8);
		m_portraitSprite.SetZ(-128);
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
	m_portraitSprite = io_game.Sprites().RemoveSprite(m_portraitSprite);

	StopMusic(io_game);

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
	case SceneMode::Voting:
	{
		Get<SceneMode::Voting>().Update();
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
		case ProgressMode::Input:
		{
			if (ABCpressedThisFrame)
			{
				m_progressMode = ProgressMode::Always;
				m_script->UpdateVN(io_game, *this);
			}
			break;
		}
		case ProgressMode::Scene:
		{
			switch (CurrentMode())
			{
			case SceneMode::None:
			{
				m_progressMode = ProgressMode::Always;
				m_script->UpdateVN(io_game, *this);
				break;
			}
			case SceneMode::Dialogue:
			{
				if (m_ABCbuffered > 0 && Get<SceneMode::Dialogue>().Done())
				{
					--m_ABCbuffered;
					m_progressMode = ProgressMode::Always;
					m_script->UpdateVN(io_game, *this);
				}
				break;
			}
			case SceneMode::Choice:
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
			case SceneMode::Voting:
			{
				if (Get<SceneMode::Voting>().VotingDone())
				{
					m_progressMode = ProgressMode::Always;
					m_script->UpdateVN(io_game, *this);
				}
			}
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
void VNWorld::WaitUntilInput
()
{
	m_progressMode = ProgressMode::Input;
}

//------------------------------------------------------------------------------
void VNWorld::StartMusic
(
	Game& io_game,
	u8 const* i_bgm,
	bool i_loop
)
{
	io_game.QueueFunctionTask([](u8 const* i_bgm, bool i_loop) -> Task {
		XGM_startPlay(i_bgm);
		XGM_setLoopNumber(i_loop ? -1 : 0);
		co_return;
	}(i_bgm, i_loop));
}

//------------------------------------------------------------------------------
void VNWorld::StopMusic
(
	Game& io_game
)
{
	io_game.QueueFunctionTask([] -> Task {
		XGM_stopPlay();
		co_return;
	}());
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
		m_bgSrc = &i_bg;
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
Task VNWorld::SetCurBGPriority
(
	bool i_priority
)
{
	if (!m_bgSrc)
	{
		return [] -> Task { co_return; }();
	}

	return Tiles::SetMap_Full(
		VDP_BG_B,
		m_bgSrc->tilemap->tilemap,
		m_bgSrc->tilemap->w,
		m_bgSrc->tilemap->h,
		TILE_ATTR_FULL(PAL0, i_priority, FALSE, FALSE, c_tilesStart)
	);
}

//------------------------------------------------------------------------------
void VNWorld::BlackBG
(
	Game& io_game,
	bool i_fast,
	bool i_setBG
)
{
	if (i_fast)
	{
		io_game.QueueLambdaTask([this, i_setBG] -> Task {
			if (i_setBG)
			{
				m_bgSrc = nullptr;
				m_bgSrcPal = palette_black;
			}
			std::copy(palette_black, palette_black + 16, m_mainPals.begin());
			std::copy(palette_black, palette_black + 16, m_namePals.begin());
			std::copy(palette_black, palette_black + 16, m_textPals.begin());

			co_return;
		});
	}
	else
	{
		io_game.QueueLambdaTask([this, i_setBG] -> Task {
			if (i_setBG)
			{
				m_bgSrc = nullptr;
				m_bgSrcPal = palette_black;
			}
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
}

consteval auto FillPaletteWhite()
{
	std::array<u16, 16> ret;
	ret.fill(RGB3_3_3_TO_VDPCOLOR(7, 7, 7));
	return ret;
}

inline constexpr std::array<u16, 16> c_palette_white = FillPaletteWhite();

//------------------------------------------------------------------------------
void VNWorld::WhiteBG
(
	Game& io_game,
	bool i_fast,
	bool i_setBG
)
{
	if (i_fast)
	{
		io_game.QueueLambdaTask([this, i_setBG] -> Task {
			if (i_setBG)
			{
				m_bgSrc = nullptr;
				m_bgSrcPal = c_palette_white.data();
			}
			std::copy(c_palette_white.begin(), c_palette_white.end(), m_mainPals.begin());
			std::copy(c_palette_white.begin(), c_palette_white.end(), m_namePals.begin());
			std::copy(c_palette_white.begin(), c_palette_white.end(), m_textPals.begin());

			co_return;
		});
	}
	else
	{
		io_game.QueueLambdaTask([this, i_setBG] -> Task {
			if (i_setBG)
			{
				m_bgSrc = nullptr;
				m_bgSrcPal = c_palette_white.data();
			}
			Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data(), c_palette_white.data(), FramesPerSecond() >> 2);
			Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data(), c_palette_white.data(), FramesPerSecond() >> 2);
			Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data(), c_palette_white.data(), FramesPerSecond() >> 2);

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
}

//------------------------------------------------------------------------------
Task VNWorld::FadeCharaPalTo
(
	u16 const* i_pal
)
{
	Palettes::FadeOp<16> fadeOp1 = Palettes::CreateFade<16>(m_mainPals.data() + 16, i_pal, FramesPerSecond() >> 1);
	Palettes::FadeOp<16> fadeOp2 = Palettes::CreateFade<16>(m_namePals.data() + 16, i_pal, FramesPerSecond() >> 1);
	Palettes::FadeOp<16> fadeOp3 = Palettes::CreateFade<16>(m_textPals.data() + 16, i_pal, FramesPerSecond() >> 1);

	while (fadeOp1)
	{
		fadeOp1.DoFadeStep();
		fadeOp2.DoFadeStep();
		fadeOp3.DoFadeStep();
		co_yield{};
	}

	co_return;
}

//------------------------------------------------------------------------------
void VNWorld::SetCharacterVisual
(
	Game& io_game,
	Pose const& i_pose
)
{
	// One big mega task, since various things are run as conditional sub tasks
	// Includes hiding existing character, if necessary
	io_game.QueueLambdaTask([this, &i_pose] -> Task {
		m_animator.StopAnimation();

		if (m_existingEndTileSet != i_pose.m_tileset)
		{
			Task wipeAway = Tiles::SetMap_Wipe<Tiles::WipeDir::Down>(
				VDP_BG_A,
				Tiles::c_emptyPlane_Highlight.data(),
				40,
				28,
				0
			);
			AwaitTask(wipeAway);
			co_yield{};
		}

		m_charaSrcPal = i_pose.m_palette->data;

		switch (CurrentMode())
		{
		case SceneMode::Dialogue:
		{
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_mainPals.begin() + 16);
			Palettes::Tint(m_mainPals.data() + 16, m_namePals.data() + 16, c_tintColour);
			Palettes::MinusOne(m_namePals.data() + 16, m_textPals.data() + 16);
			break;
		}
		case SceneMode::Choice:
		{
			Palettes::Tint(m_charaSrcPal, m_mainPals.data() + 16, c_tintColour);
			std::copy(m_mainPals.begin() + 16, m_mainPals.begin() + 32, m_namePals.begin() + 16);
			std::copy(m_mainPals.begin() + 16, m_mainPals.begin() + 32, m_textPals.begin() + 16);
			break;
		}
		case SceneMode::None:
		case SceneMode::Voting:
		{
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_mainPals.begin() + 16);
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_namePals.begin() + 16);
			std::copy(m_charaSrcPal, m_charaSrcPal + 16, m_textPals.begin() + 16);
			break;
		}
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
				AnimFrame const& firstAnimFrame = i_pose.m_animation[0];
				Task wipe = Tiles::SetMap_Wipe<Tiles::WipeDir::Up>(
					VDP_BG_A,
					firstAnimFrame.m_tilemap->tilemap,
					firstAnimFrame.m_tilemap->w,
					firstAnimFrame.m_tilemap->h,
					baseTile,
					firstAnimFrame.m_xOffset,
					firstAnimFrame.m_yOffset
				);
				AwaitTask(wipe);
			}
		}
		else
		{
			// If using same tilemap, need to set first frame still
			AnimFrame const& firstAnimFrame = i_pose.m_animation[0];
			Task firstFrame = Tiles::SetMap_SubFull(
				VDP_BG_A,
				firstAnimFrame.m_tilemap->tilemap,
				firstAnimFrame.m_tilemap->w,
				firstAnimFrame.m_tilemap->h,
				baseTile,
				firstAnimFrame.m_xOffset,
				firstAnimFrame.m_yOffset
			);
			AwaitTask(firstFrame);
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
		m_existingEndTileSet = nullptr;
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
	SFXID i_beeps
)
{
	TransitionTo(io_game, SceneMode::Dialogue);

	if (i_char)
	{
		Get<SceneMode::Dialogue>().SetName(i_char->m_displayName, i_char->m_showOnLeft);
	}
	else
	{
		Get<SceneMode::Dialogue>().SetName(nullptr, true);
	}

	Get<SceneMode::Dialogue>().SetText(i_text, i_beeps, i_char ? i_char->m_useDescFont : false);
}

//------------------------------------------------------------------------------
void VNWorld::SetPortrait
(
	Game& io_game,
	PortraitFace const& i_face
)
{
	io_game.QueueLambdaTask([this, &io_game, &i_face] -> Task {
		{
			m_portraitSprite.SetVisible(false);
		}

		PAL_setColors(16 * PAL2, i_face.m_pal->data, 16, DMA_QUEUE);

		// Should load the whole chunk in one go, may not even yield!
		auto load = Tiles::LoadTiles_Chunked(i_face.m_tiles, c_reservedPortraitTileStart);
		AwaitTask(load);

		// May have yielded, so need to request again as iter may be invalidated
		{
			m_portraitSprite.SetVisible(true);
		}

		co_return;
	});
}

//------------------------------------------------------------------------------
void VNWorld::HidePortrait
(
	Game& io_game
)
{
	io_game.QueueLambdaTask([this, &io_game] -> Task {
		m_portraitSprite.SetVisible(false);

		co_return;
	});
}

//------------------------------------------------------------------------------
void VNWorld::Choice
(
	Game& io_game,
	std::span<char const* const> i_choices
)
{
	TransitionTo(io_game, SceneMode::Choice);

	io_game.QueueLambdaTask([this, i_choices] -> Task {
		Get<SceneMode::Choice>().SetChoices(i_choices);
		co_return;
	});
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

	io_game.QueueLambdaTask([this, i_choices, i_timeInSeconds] -> Task {
		Get<SceneMode::Choice>().SetChoices(i_choices, i_timeInSeconds);
		co_return;
	});
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
void VNWorld::ClearModeImmediate
(
	Game& io_game
)
{
	HidePortrait(io_game);
	m_sceneMode.emplace<static_cast<u8>(SceneMode::None)>();
	m_progressMode = ProgressMode::Always;
}

//------------------------------------------------------------------------------
void VNWorld::StartVote
(
	Game& io_game,
	VoteModeParams const& i_params
)
{
	TransitionTo(io_game, SceneMode::Voting);

	io_game.QueueLambdaTask([this, i_params] -> Task
	{
		Get<SceneMode::Voting>().Start(std::move(i_params));
		co_return;
	});
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
		// Always update the progress mode even if scene does not need transitioning
		m_progressMode = i_sceneMode == SceneMode::None
			? ProgressMode::Always
			: ProgressMode::Scene
		;
		return;
	}

	switch (i_sceneMode)
	{
	case SceneMode::None:
	{
		HidePortrait(io_game);

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

			EnableMidScreenPaletteSwaps(false);
		});
		WaitForTasks(io_game);

		m_sceneMode.emplace<static_cast<u8>(SceneMode::None)>();
		m_progressMode = ProgressMode::Always;
		break;
	}
	case SceneMode::Dialogue:
	{
		io_game.QueueLambdaTask([this] -> Task {
			EnableMidScreenPaletteSwaps(true);

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
		WaitForTasks(io_game);

		m_sceneMode.emplace<static_cast<u8>(SceneMode::Dialogue)>(io_game, m_fonts);
		m_progressMode = ProgressMode::Scene;
		break;
	}
	case SceneMode::Choice:
	{
		HidePortrait(io_game);

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

			EnableMidScreenPaletteSwaps(false);
		});
		WaitForTasks(io_game);

		m_sceneMode.emplace<static_cast<u8>(SceneMode::Choice)>(io_game, m_fonts);
		m_progressMode = ProgressMode::Scene;
		break;
	}
	case SceneMode::Voting:
	{
		HideCharacterVisual(io_game, false);
		HidePortrait(io_game);

		io_game.QueueLambdaTask([this] -> Task {
			// Just tint the background, chara will be gone
			std::array<u16, 16> darkPal;
			Palettes::Tint<16>(m_bgSrcPal, darkPal.data(), c_tintColour);

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

			EnableMidScreenPaletteSwaps(false);
		});
		WaitForTasks(io_game);

		m_sceneMode.emplace<static_cast<u8>(SceneMode::Voting)>(io_game, m_fonts, *this);
		m_progressMode = ProgressMode::Scene;
		break;
	}
	}
}

//------------------------------------------------------------------------------
void VNWorld::EnableMidScreenPaletteSwaps
(
	bool i_enable
)
{
	if (i_enable)
	{
		HInt_TextArea_SetName();
	}
	else
	{
		HInt_TextArea_Reset_Only();
	}
}

}