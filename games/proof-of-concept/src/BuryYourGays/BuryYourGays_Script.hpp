#pragma once

#include "Declare.hpp"
#include "Script.hpp"
#include "Scene.hpp"
#include "BuryYourGays_Scenes.hpp"

#include <memory>
#include <optional>

namespace BuryYourGays
{

enum class Variables : u8
{
	SceneNum,
	StaceyApproval,

	Count,
};

class Script
	: public Game::Script
{
	void InitVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	void UpdateVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	void SetNextScene(Scenes i_scene) { m_nextScene = i_scene; }
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::optional<Scenes> m_nextScene;
};

}