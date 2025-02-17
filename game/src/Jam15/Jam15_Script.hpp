#pragma once

#include "Declare.hpp"
#include "Script.hpp"
#include "Scene.hpp"
#include "Jam15_Scenes.hpp"

#include <memory>
#include <optional>

namespace Jam15
{

enum class Variables : u8
{
	SceneNum,

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

	Game::SpriteID m_arrowSpr;
	s16 m_selection{ 0 };

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::optional<Scenes> m_nextScene;

public:
	Game::NoSFX zanmu_beeps;
	Game::NoSFX hisami_beeps;
	Game::NoSFX suika_beeps;
	Game::NoSFX yuugi_beeps;
	Game::NoSFX yuuma_beeps;
	Game::NoSFX kishin_beeps;
};

}