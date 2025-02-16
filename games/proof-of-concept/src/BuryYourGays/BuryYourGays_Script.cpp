#include "BuryYourGays_Script.hpp"

#include "Game.hpp"

namespace BuryYourGays
{

void Script::Init
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

	io_game.SaveVariables();

	m_nextScene = CreateScene(sceneToStart);
	Update(io_game, io_vn);
}

void Script::Update
(
	Game::Game& io_game,
	Game::VNWorld& io_vn
)
{
	if (m_nextScene)
	{
		m_currentScene = std::move(m_nextScene);
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