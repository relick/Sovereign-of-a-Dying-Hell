#pragma once

#include "Declare.hpp"
#include "CharacterAnimator.hpp"
#include "ChoiceSystem.hpp"
#include "DialoguePrinter2.hpp"
#include "FontData.hpp"
#include "VoteMode.hpp"
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

	u16 const* m_bgSrcPal{ palette_black };
	u16 const* m_charaSrcPal{ palette_black };
	std::array<u16, 32> m_mainPals{};
	std::array<u16, 32> m_namePals{};
	std::array<u16, 32> m_textPals{};

	TileSet const* m_existingEndTileSet{ nullptr }; // Usually a character's tiles, but should be set based on any source

	u8 m_waitingForTasksStack{ 0 };

	// Mostly changes which subsystem is active/allows transitions - should be mutually exclusive
	enum class SceneMode : u8
	{
		None,
		Dialogue,
		Choice,
		Voting,
	};
	using SceneModeVar = std::variant<
		std::monostate,
		DialoguePrinter2,
		ChoiceSystem,
		VoteMode
	>;
	SceneModeVar m_sceneMode;

	// Determines how the scene routine is progressed
	enum class ProgressMode : u8
	{
		Always, // Reset to this after progress made via any way. In particular used by wait_for_tasks()
		Scene, // Waits for current scene mode subsystem to indicate progress
	};
	ProgressMode m_progressMode{ ProgressMode::Always };

	// Buttons
	bool m_ABCpressed{ false }; // Used to force letting go before pressing again
	u8 m_ABCbuffered{ 0 }; // Used to receive input to be processed on a task free frame. 0 = no press, >0 = number of presses
	u8 m_ABCbufferTimer{ 0 }; // Cuts buffered input early if this time hits 0.

	// Choice data
	std::optional<u8> m_choiceMade;

	// Portrait
	SpriteID m_portraitSprite{};

public:
	VNWorld(std::unique_ptr<Script> i_script);

	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;

	void WaitForTasks(Game& io_game);
	void StartMusic(u8 const* i_bgm, u16 i_fadeInFrames, bool i_loop);
	void StopMusic(u16 i_fadeOutFrames);
	void SetBG(Game& io_game, Image const& i_bg);
	void BlackBG(Game& io_game);
	void SetCharacterVisual(Game& io_game, Pose const& i_pose);
	void HideCharacterVisual(Game& io_game, bool i_fast);
	void SetPortrait(Game& io_game, PortraitFace const& i_face);
	void HidePortrait(Game& io_game);
	bool CharacterShown() const { return m_existingEndTileSet != nullptr; }
	void SetText(Game& io_game, Character const* i_char, char const* i_text, std::optional<SFXID> i_beeps); // null char = hide name
	void Choice(Game& io_game, std::span<char const* const> i_choices);
	void TimedChoice(Game& io_game, std::span<char const* const> i_choices, f16 i_timeInSeconds);
	std::optional<u8> GetChoiceResult() const { return m_choiceMade; }
	void ClearMode(Game& io_game);

private:
	SceneMode CurrentMode() const { return static_cast<SceneMode>(m_sceneMode.index()); }
	void TransitionTo(Game& io_game, SceneMode i_sceneMode);

	template<SceneMode t_SceneMode>
	auto Get() -> decltype(std::get<static_cast<u8>(t_SceneMode)>(m_sceneMode))
	{
		return std::get<static_cast<u8>(t_SceneMode)>(m_sceneMode);
	}
};

}