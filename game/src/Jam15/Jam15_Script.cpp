#include "Jam15_Script.hpp"

#include "FadeOps.hpp"
#include "Game.hpp"
#include "GameRoutines.hpp"
#include "TileOps.hpp"
#include "TitleWorld.hpp"
#include "Version.hpp"
#include "VNWorld.hpp"

#include "Jam15/res_bg.h"
#include "res_vn.h"
#include "Jam15/res_sfx.h"

namespace Jam15
{

void Script::InitTitle
(
	Game::Game& io_game,
	Game::TitleWorld& io_title
)
{
	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(beach2.tileset, c_tilesStart));
	io_game.QueueFunctionTask(Tiles::ClearMap_Full(
		VDP_BG_A,
		Tiles::c_emptyPlane
	));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(VDP_BG_B, beach2.tilemap->tilemap, beach2.tilemap->w, beach2.tilemap->h, c_tilesStart));
	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(title.tileset, c_tilesEnd - title.tileset->numTile));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(VDP_BG_A, title.tilemap->tilemap, title.tilemap->w, title.tilemap->h,
		TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, c_tilesEnd - title.tileset->numTile)
	));
	io_game.QueueLambdaTask([] -> Game::Task {
		std::array<u16, 32> pal;
		Palettes::FadeOp<16> fade = Palettes::CreateFade<16>(pal.data(), beach2.palette->data, FramesPerSecond());
		Palettes::FadeOp<16> fade2 = Palettes::CreateFade<16>(pal.data() + 16, title.palette->data, FramesPerSecond());

		while (fade)
		{
			fade.DoFadeStep();
			fade2.DoFadeStep();
			PAL_setColors(0, pal.data(), 32, DMA_QUEUE);
			co_yield{};
		}

		co_return;
	});

	u16 const arrowTiles = io_game.Sprites().InsertMiscTiles(misc_spr);
	auto [arrowID, spr] = io_game.Sprites().AddSprite(Game::SpriteSize::r1c1, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, arrowTiles + 1));
	m_arrowSpr = arrowID;
	spr.SetX(8 * 5);
	spr.SetY(8 * 21);
}

void Script::UpdateTitle
(
	Game::Game& io_game,
	Game::TitleWorld& io_title
)
{
	static bool pressed = false;

	u16 const buttons = JOY_readJoypad(JOY_1);
	if ((buttons & BUTTON_A) != 0)
	{
		if (m_selection == 0)
		{
			io_game.LoadVariables();
		}

		io_title.GoToVNWorld(io_game);
		return;
	}
	else if ((buttons & BUTTON_DOWN) != 0)
	{
		if (!pressed)
		{
			++m_selection;
			if (m_selection > 1)
			{
				m_selection = 0;
			}

			auto spr = io_game.Sprites().EditSpriteData(m_arrowSpr);
			spr.SetY(8 * 21 + m_selection * 24);
		}
		pressed = true;
	}
	else if ((buttons & BUTTON_UP) != 0)
	{
		if (!pressed)
		{
			--m_selection;
			if (m_selection < 0)
			{
				m_selection = 1;
			}
			auto spr = io_game.Sprites().EditSpriteData(m_arrowSpr);
			spr.SetY(8 * 21 + m_selection * 24);
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
	// zanmu_beeps = io_game.SFX().AddSFX(mid_beep, std::size(mid_beep), Game::Channel::TextBeeps);

	// First scene
	Scenes sceneToStart = Scenes::scene1;

	// Set up variables
	if (io_game.HasLoadedData())
	{
		// Get scene num
		sceneToStart = io_game.ReadVar<Scenes>(Variables::SceneNum);
	}
	else
	{
		io_game.SetVariableCount(static_cast<u16>(Variables::Count));
	}

	m_nextScene = sceneToStart;
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
		io_game.SetVar<Scenes>(Variables::SceneNum, *m_nextScene);
		io_game.SaveVariables();

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
	case Scenes::scene1: return std::make_unique<scene1>();
	}
	
	return {};
}

}