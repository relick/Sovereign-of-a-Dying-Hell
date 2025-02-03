#pragma once

#include "Declare.hpp"
#include "CharacterAnimator.hpp"
#include "ChoiceSystem.hpp"
#include "DialoguePrinter2.hpp"
#include "FontData.hpp"
#include "Worlds.hpp"

#include <array>
#include <memory>
#include <variant>

namespace Game
{

class VNWorld
	: public World
{
	FontData m_fonts;
	CharacterAnimator m_animator;

	std::unique_ptr<Script> m_script;

	Image const* m_nextBG{ nullptr };
	Pose const* m_nextPose{ nullptr };

	u16 const* m_bgSrcPal{ palette_black };
	u16 const* m_charaSrcPal{ palette_black };
	std::array<u16, 32> m_mainPals{};
	std::array<u16, 32> m_namePals{};
	std::array<u16, 32> m_textPals{};

	u8 m_waitingForTasksStack{ 0 };

	// Mostly changes which subsystem is active/allows transitions - should be mutually exclusive
	enum class SceneMode : u8
	{
		None,
		Dialogue,
		Choice,
		Settings,
	};
	using SceneModeVar = std::variant<
		std::monostate,
		DialoguePrinter2,
		ChoiceSystem,
		std::monostate
	>;
	SceneModeVar m_sceneMode;

	// Determines how the scene routine is progressed
	enum class ProgressMode : u8
	{
		Always, // Reset to this after progress made via any way. In particular used by wait_for_tasks()
		Dialogue, // Waits for dialogue printer to indicate progress can happen, and player to progress
		Choice, // Waits for choice subsystem to return a choice
	};
	ProgressMode m_progressMode{ ProgressMode::Always };

	// Buttons
	bool m_ABCpressed{ false };

	// Choice data
	std::optional<u8> m_choiceMade;

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
	void SetCharacterVisual(Game& io_game, Pose const& i_pose);
	void HideCharacterVisual(Game& io_game, bool i_fast);
	void SetText(Game& io_game, Character const* i_char, char const* i_text); // null char = hide name
	void Choice(Game& io_game, std::span<char const* const> i_choices);
	void TimedChoice(Game& io_game, std::span<char const* const> i_choices, f16 i_timeInSeconds);
	std::optional<u8> GetChoiceResult() const { return m_choiceMade; }

	SceneMode CurrentMode() const { return static_cast<SceneMode>(m_sceneMode.index()); }
	void TransitionTo(Game& io_game, SceneMode i_sceneMode);

	template<SceneMode t_SceneMode>
	auto Get() -> decltype(std::get<static_cast<u8>(t_SceneMode)>(m_sceneMode))
	{
		return std::get<static_cast<u8>(t_SceneMode)>(m_sceneMode);
	}
};

}