#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/Script.hpp"
#include "adventure-md/Scene.hpp"
#include "TestZanmu_Scenes.hpp"

#include <memory>

namespace TestZanmu
{

class Script
	: public Game::Script
{
	void InitVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	void UpdateVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::unique_ptr<Game::Scene> m_nextScene;

public:
	Game::SFXID zanmu_beeps;
};

}