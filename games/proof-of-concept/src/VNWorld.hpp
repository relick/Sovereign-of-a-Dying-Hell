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

	Image const* m_nextBG{ nullptr };
	Pose const* m_nextPose{ nullptr };

	u16 const* m_bgSrcPal{ nullptr };
	u16 const* m_charaSrcPal{ nullptr };
	std::array<u16, 32> m_mainPals{};
	std::array<u16, 32> m_namePals{};
	std::array<u16, 32> m_textPals{};

	u8 m_waitingForTasksStack{ 0 };

	// Mostly changes which subsystem is active/allows transitions - should be mutually exclusive
	enum class SceneMode
	{
		None,
		Dialogue,
		Choice,
		Settings,
	};
	SceneMode m_sceneMode{ SceneMode::Dialogue };

	// Determines how the scene routine is progressed
	enum class ProgressMode
	{
		Always, // Reset to this after progress made via any way. In particular used by wait_for_tasks()
		Dialogue, // Waits for dialogue printer to indicate progress can happen, and player to progress
		Choice, // Waits for choice subsystem to return a choice
	};
	ProgressMode m_progressMode{ ProgressMode::Always };

	// Buttons
	bool m_ABCpressed{ false };

	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;

public:
	VNWorld(std::unique_ptr<Script>&& i_script);

	void WaitForTasks(Game& io_game);
	void StartMusic(u8 const* i_bgm, u16 i_fadeInFrames, bool i_loop);
	void StopMusic(u16 i_fadeOutFrames);
	void SetBG(Game& io_game, Image const& i_bg);
	void BlackBG(Game& io_game);
	void SetCharacter(Game& io_game, CharacterID i_charID, PoseID i_poseID);
	void HideCharacter(Game& io_game, bool i_fast);
	void SetCharName(CharacterID i_charID);
	void SetText(char const* i_text);
};

}