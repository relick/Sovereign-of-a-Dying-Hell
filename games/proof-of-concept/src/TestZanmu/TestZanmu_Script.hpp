#pragma once

#include "Declare.hpp"
#include "Script.hpp"
#include "Scene.hpp"
#include "TestZanmu_Scenes.hpp"

#include <memory>

namespace TestZanmu
{

class Script
	: public Game::Script
{
	void Init(Game::Game& io_game, Game::VNWorld& io_vn) override;
	void Update(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::unique_ptr<Game::Scene> m_nextScene;
};

}