#pragma once

#include "Declare.hpp"
#include "Game.hpp"
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
	Influence,
	HasYuugiInfluence,
	PunishmentVotePasses,
	SuikaDissuaded,
	PriceIncreaseVoteFails,

	Count,
};
inline constexpr u16 c_saveVersion = static_cast<u16>(Variables::Count); // Just hacking it as count for now while in dev

}

template<>
struct Game::VariablesTypeTuple<Jam15::Variables> {
	using Tuple = std::tuple<
		Jam15::Scenes, // SceneNum
		u16, // Influence
		bool, // HasYuugiInfluence
		bool, // PunishmentVotePasses
		bool, // SuikaDissuaded
		bool // PriceIncreaseVoteFails
	>;
};

namespace Jam15
{

class Script
	: public Game::Script
{
	void InitTitle(Game::Game& io_game, Game::TitleWorld& io_title) override;
	void UpdateTitle(Game::Game& io_game, Game::TitleWorld& io_title) override;

	void InitVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	void UpdateVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	Game::SpriteID m_arrowSpr;
	s16 m_selection{ 0 };
	bool m_hasLoadedData{ false };

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::optional<Scenes> m_nextScene;

public:
	void SetNextScene(Scenes i_scene) { m_nextScene = i_scene; }

	Game::SFXID desc_beeps;

	Game::SFXID zanmu_beeps;
	Game::SFXID hisami_beeps;
	Game::SFXID suika_beeps;
	Game::SFXID yuugi_beeps;
	Game::SFXID yuuma_beeps;
	Game::SFXID acouncil_beeps;
	Game::SFXID bcouncil_beeps;
	Game::SFXID ccouncil_beeps;
	Game::SFXID dcouncil_beeps;
	Game::SFXID speaker_beeps;
	Game::SFXID kishin_beeps;
};

}