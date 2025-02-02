#include "BuryYourGays_Script.hpp"

namespace BuryYourGays
{

void Script::Init
(
	Game::Game& io_game,
	Game::VNWorld& io_vn
)
{
	// First scene
	m_nextScene = CreateScene(Scenes::forestShed_runningThroughWoods);
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