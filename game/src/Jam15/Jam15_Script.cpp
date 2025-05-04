#include "Jam15_Script.hpp"

#include "FadeOps.hpp"
#include "GameRoutines.hpp"
#include "TileOps.hpp"
#include "TitleWorld.hpp"
#include "Version.hpp"
#include "VNWorld.hpp"

#include "Jam15/res_bg.h"
#include "res_vn.h"
#include "Jam15/res_sfx.h"
#include "res_fonts.h"

namespace Jam15
{

void Script::InitTitle
(
	Game::Game& io_game,
	Game::TitleWorld& io_title
)
{
	io_game.LoadVariables(c_saveVersion);
	m_hasLoadedData = io_game.HasLoadedData();
	if (!m_hasLoadedData)
	{
		m_selection = 1;
	}
	else
	{
		m_isMashlessMode = io_game.ReadVar<Variables::EasyMode>();
	}

	Image const& titleImg = m_hasLoadedData ? title : title_nocont;

	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(hell_palace.tileset, c_tilesStart));
	io_game.QueueFunctionTask(Tiles::ClearMap_Full(
		VDP_BG_A,
		Tiles::c_emptyPlane
	));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(VDP_BG_B, hell_palace.tilemap->tilemap, hell_palace.tilemap->w, hell_palace.tilemap->h, c_tilesStart));
	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(titleImg.tileset, c_tilesEnd - titleImg.tileset->numTile));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(VDP_BG_A, titleImg.tilemap->tilemap, titleImg.tilemap->w, titleImg.tilemap->h,
		TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, c_tilesEnd - titleImg.tileset->numTile)
	));
	io_game.QueueLambdaTask([this, &io_game, &titleImg] -> Game::Task {
		std::array<u16, 32> pal{};
		Palettes::FadeOp<16> fade = Palettes::CreateFade<16>(pal.data(), hell_palace.palette->data, FramesPerSecond());
		Palettes::FadeOp<16> fade2 = Palettes::CreateFade<16>(pal.data() + 16, titleImg.palette->data, FramesPerSecond());

		while (fade)
		{
			fade.DoFadeStep();
			fade2.DoFadeStep();
			PAL_setColors(0, pal.data(), 32, DMA_QUEUE);
			co_yield{};
		}

		u16 const arrowTiles = io_game.Sprites().InsertMiscTiles(misc_spr);
		PAL_setColors(16 * PAL3, text_font_pal.data, 16, DMA_QUEUE);
		m_arrowSpr = io_game.Sprites().AddSprite(Game::SpriteSize::r1c1, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, arrowTiles + 1));
		m_arrowSpr.SetX(8 * 5);
		m_arrowSpr.SetY(156 + m_selection * 21);

		co_return;
	});
}

void Script::UpdateTitle
(
	Game::Game& io_game,
	Game::TitleWorld& io_title
)
{
	static bool pressed = false;

	u16 const buttons = JOY_readJoypad(JOY_1);
	if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
	{
		if (m_selection != 0)
		{
			m_hasLoadedData = false;

			m_isMashlessMode = m_selection == 2;
			io_game.SetVar<Variables::EasyMode>(m_isMashlessMode);
		}

		m_arrowSpr = io_game.Sprites().RemoveSprite(m_arrowSpr);
		io_title.GoToVNWorld(io_game);
		return;
	}
	else if ((buttons & BUTTON_DOWN) != 0)
	{
		if (!pressed)
		{
			++m_selection;
			if (m_selection > 2)
			{
				m_selection = m_hasLoadedData ? 0 : 1;
			}
			m_arrowSpr.SetY(156 + m_selection * 21);
		}
		pressed = true;
	}
	else if ((buttons & BUTTON_UP) != 0)
	{
		if (!pressed)
		{
			--m_selection;
			if (m_selection < (m_hasLoadedData ? 0 : 1))
			{
				m_selection = 2;
			}
			m_arrowSpr.SetY(156 + m_selection * 21);
		}
		pressed = true;
	}
	else
	{
		pressed = false;
	}
}

void Script::InitVN
(
	Game::Game& io_game,
	Game::VNWorld& io_vn
)
{
	// Set up data
	desc_beeps = io_game.SFX().AddSFX(zanmu_beep, std::size(zanmu_beep), Game::Channel::TextBeeps);
	zanmu_beeps = desc_beeps;
	hisami_beeps = desc_beeps;
	suika_beeps = desc_beeps;
	yuugi_beeps = desc_beeps;
	yuuma_beeps = desc_beeps;
	acouncil_beeps = desc_beeps;
	bcouncil_beeps = desc_beeps;
	ccouncil_beeps = desc_beeps;
	dcouncil_beeps = desc_beeps;
	speaker_beeps = desc_beeps;
	kishin_beeps = desc_beeps;

	// First scene
	Scenes sceneToStart = Scenes::InitialScene;

	// Set up variables
	if (m_hasLoadedData)
	{
		// Get scene num
		sceneToStart = io_game.ReadVar<Variables::SceneNum>();
	}
	else
	{
		io_game.ResetVariables<Variables>();
	}

	SetNextScene(sceneToStart);
	UpdateVN(io_game, io_vn);
}

void Script::UpdateVN
(
	Game::Game& io_game,
	Game::VNWorld& io_vn
)
{
	if (m_nextScene)
	{
		// Don't save if we're entering an ending scene
		if (!m_nextSceneIsEnding)
		{
			// Save data first
			io_game.SetVar<Variables::SceneNum>(*m_nextScene);
			io_game.SaveVariables(c_saveVersion);
		}

		// Tidy up visuals
		io_vn.HideCharacterVisual(io_game, false);
		io_vn.ClearModeImmediate(io_game);
		io_vn.BlackBG(io_game, true, true);

		m_currentScene = CreateScene(*m_nextScene);
		m_nextScene = std::nullopt;
		m_currentSceneRoutine = m_currentScene->Run(io_game, io_vn, *this);
	}
	else if (m_currentSceneRoutine)
	{
		m_currentSceneRoutine();
	}
}

std::unique_ptr<Game::Scene> Script::CreateScene
(
	Scenes i_scene
)
{
	switch(i_scene)
	{
	case Scenes::FirstVoteProposal: return std::make_unique<FirstVoteProposal>();
	case Scenes::HatchingThePlan: return std::make_unique<HatchingThePlan>();
	case Scenes::LobbyingYuugi: return std::make_unique<LobbyingYuugi>();
	case Scenes::VotingForAnimalRights: return std::make_unique<VotingForAnimalRights>();
	case Scenes::SuikaApproaches: return std::make_unique<SuikaApproaches>();
	case Scenes::DelegatingToHisami: return std::make_unique<DelegatingToHisami>();
	case Scenes::VotingForPriceIncreases: return std::make_unique<VotingForPriceIncreases>();
	//case Scenes::MeetingTheShadowyKishin: return std::make_unique<MeetingTheShadowyKishin>();
	case Scenes::EngagingYuuma: return std::make_unique<EngagingYuuma>();
	case Scenes::VotingForExecutive: return std::make_unique<VotingForExecutive>();
	case Scenes::FinalHisami: return std::make_unique<FinalHisami>();
	//case Scenes::FinalYuugi: return std::make_unique<FinalYuugi>();
	//case Scenes::FinalSuika: return std::make_unique<FinalSuika>();
	//case Scenes::FinalYuuma: return std::make_unique<FinalYuuma>();
	case Scenes::VotingToRelocateHell: return std::make_unique<VotingToRelocateHell>();

	case Scenes::Ending_RulingDecay: return std::make_unique<Ending_RulingDecay>();
	case Scenes::Ending_NewHell: return std::make_unique<Ending_NewHell>();
	case Scenes::Ending_AdministratingCollapse: return std::make_unique<Ending_AdministratingCollapse>();

	case Scenes::CharacterViewer: return std::make_unique<CharacterViewer>();
	}
	
	return {};
}

}