#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/Script.hpp"
#include "adventure-md/Scene.hpp"
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
	void InitTitle(Game::Game& io_game, Game::TitleWorld& io_title) override;
	void UpdateTitle(Game::Game& io_game, Game::TitleWorld& io_title) override;

	void InitVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	void UpdateVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	void SetNextScene(Scenes i_scene) { m_nextScene = i_scene; }
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	Game::SpriteHandle m_arrowSpr;
	s16 m_selection{ 0 };

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::optional<Scenes> m_nextScene;

public:
	Game::SFXID stacey_beeps;
	Game::SFXID beans_beeps;

};

}