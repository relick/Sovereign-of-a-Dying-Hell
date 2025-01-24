#pragma once

#include "Declare.hpp"
#include "Constants.hpp"
#include "Worlds.hpp"
#include "DialoguePrinter2.hpp"
#include "CharacterData.hpp"
#include "Debug.hpp"

#include <genesis.h>

#include <array>
#include <memory>

namespace Game
{

class VNWorld
	: public World
{
	CharacterData m_characters;
	DialoguePrinter2 m_printer;

	std::unique_ptr<Script> m_script;
	bool m_readyForNext{false};

	Image const* m_nextBG{ nullptr };
	Pose const* m_nextPose{ nullptr };

	std::array<u16, 16> m_bgNameCalcPal;
	std::array<u16, 16> m_bgTextCalcPal;

	std::array<u16, 16> m_charaNameCalcPal;
	std::array<u16, 16> m_charaTextCalcPal;

	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;

public:
	VNWorld(std::unique_ptr<Script>&& i_script);

	void StartMusic(u8 const* i_bgm, u16 i_fadeInFrames, bool i_loop);
	void StopMusic(u16 i_fadeOutFrames);
	void SetBG(Game& io_game, Image const& i_bg);
	void BlackBG(Game& io_game);
	void SetCharacter(Game& io_game, char const* i_charName, char const* i_poseName);
	void HideCharacter(Game& io_game, bool i_fast);
	void SetCharName(char const* i_charName);
	void SetText(char const* i_text);
};

}