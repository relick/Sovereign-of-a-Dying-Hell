#pragma once

#include "Script.hpp"
#include "Scene.hpp"
#include "BuryYourGays_Scenes.hpp"

#include <memory>

namespace BuryYourGays
{

class Script
	: public Game::Script
{
	void Init(Game::Game& io_game, Game::VNWorld& io_vn, Game::CharacterData& io_cd) override;
	void Update(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::unique_ptr<Game::Scene> m_nextScene;

public:
	
};

}