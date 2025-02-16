#include "BuryYourGays_Script.hpp"

#include "FadeOps.hpp"
#include "Game.hpp"
#include "GameRoutines.hpp"
#include "TileOps.hpp"
#include "TitleWorld.hpp"
#include "Version.hpp"
#include "VNWorld.hpp"

#include "res_bg.h"

namespace BuryYourGays
{

void Script::InitTitle
(
	Game::Game& io_game,
	Game::TitleWorld& io_title
)
{
	io_game.QueueFunctionTask(Tiles::LoadTiles_Chunked(forest.tileset, c_tilesStart));
	io_game.QueueFunctionTask(Tiles::ClearMap_Full(
		VDP_BG_A,
		Tiles::c_emptyPlane
	));
	io_game.QueueFunctionTask(Tiles::SetMap_Full(VDP_BG_B, forest.tilemap->tilemap, forest.tilemap->w, forest.tilemap->h, c_tilesStart));
	io_game.QueueLambdaTask([] -> Game::Task {
		std::array<u16, 16> bgPal;
		Palettes::FadeOp<16> fade = Palettes::CreateFade<16>(bgPal.data(), forest.palette->data, FramesPerSecond());

		while (fade)
		{
			fade.DoFadeStep();
			PAL_setColors(0, bgPal.data(), 16, DMA_QUEUE);
			co_yield{};
		}

		co_return;
	});
}

void Script::UpdateTitle
(
	Game::Game& io_game,
	Game::TitleWorld& io_title
)
{
	u16 const buttons = JOY_readJoypad(JOY_1);
	if ((buttons & BUTTON_A) != 0)
	{
		io_title.GoToVNWorld(io_game);
		return;
	}
}

void Script::InitVN
(
	Game::Game& io_game,
	Game::VNWorld& io_vn
)
{
	io_game.LoadVariables();

	// First scene
	Scenes sceneToStart = Scenes::forestShed_runningThroughWoods;

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
		case Scenes::forestShed_runningThroughWoods: return std::make_unique<forestShed_runningThroughWoods>();
	}
	
	return {};
}

}