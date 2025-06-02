#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/Game.hpp"
#include "adventure-md/Saves.hpp"
#include "adventure-md/Script.hpp"
#include "adventure-md/Scene.hpp"
#include "adventure-md/SpriteManager.hpp"
#include "Jam15_Scenes.hpp"

#include <memory>
#include <optional>

namespace Jam15
{

struct Variables
{
	Scenes SceneNum{};
	u16 Influence{};
	bool HasYuugiInfluence{};
	bool PunishmentVotePasses{};
	bool SuikaDissuaded{};
	bool LobbiedTheYama{};
	bool SpreadSeedsOfDoubt{};
	bool KishinAlliance{};
	bool PriceIncreaseVoteFails{};
	bool YuumaPromised{};
	bool MashlessMode{};
};
enum class SaveVersion : u8
{
	NewSaveSystem = 1,

	End,
	Current = End - 1,
	OldestSupported = NewSaveSystem,
};

// 16 available slots (1KiB each, overkill I guess)
using SRAM = Saves::SaveMemory<16>;
inline constexpr u16 c_saveSlot = 0; // TODO(?): more than one save slot

class Script
	: public Game::Script
{
	void InitTitle(Game::Game& io_game, Game::TitleWorld& io_title) override;
	void UpdateTitle(Game::Game& io_game, Game::TitleWorld& io_title) override;

	void InitVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	void UpdateVN(Game::Game& io_game, Game::VNWorld& io_vn) override;
	
	std::unique_ptr<Game::Scene> CreateScene(Scenes i_scene);

	Variables m_variables;

	Game::SpriteHandle m_arrowSpr;
	s16 m_selection{ 0 };
	bool m_hasLoadedData{ false };

	std::unique_ptr<Game::Scene> m_currentScene;
	Game::SceneRoutine m_currentSceneRoutine;
	std::optional<Scenes> m_nextScene;
	bool m_nextSceneIsEnding{ false };
	
	void SaveVariables();
	void TryLoadVariables();

public:
	bool IsMashlessMode() const { return m_variables.MashlessMode; }
	void SetNextScene(Scenes i_scene) { m_nextScene = i_scene; }
	void GoToEnding(Scenes i_scene) { m_nextScene = i_scene; m_nextSceneIsEnding = true; }

	Variables& Vars() { return m_variables; }

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