#pragma once

#include "Declare.hpp"
#include "GameRoutines.hpp"
#include "TileData.hpp"

#include <array>
#include <expected>
#include <optional>
#include <span>
#include <vector>

namespace Game
{

//------------------------------------------------------------------------------
/// Displays a set of choices with variable-width text and manages input
//------------------------------------------------------------------------------
class ChoiceSystem
{
	Game* m_game{ nullptr };
	FontData const* m_fonts{ nullptr };

	bool m_vramInitialised{ false };
	bool m_spritesInitialised{ false };

	std::span<char const* const> m_choices;
	std::optional<f16> m_timeLimit{ 0 };

	VBlankCallbackID m_dmaCallbackID{};

	std::array<Tiles::Tile, 128> m_tiles{};

	// 4 sprites per choice i.e. 128 pixels
	std::vector<SpriteID> m_choiceTextSprites;
	SpriteID m_choiceArrow;
	s16 m_baseChoiceY{ 0 };
	u16 m_floatScroll{ 0 };
	s16 m_float{ 0 };

	u8 m_highlightedChoice{ 0 };

public:
	ChoiceSystem(Game& io_game, FontData const& i_fonts);
	~ChoiceSystem();

	void SetChoices(std::span<char const* const> i_choices, std::optional<f16> i_timeLimitInSeconds = std::nullopt);

	// expected: index of choice made
	enum class NoChoiceMade { Waiting, TimeLimitReached, };
	std::expected<u8, NoChoiceMade> Update(bool i_choosePressed);

private:
	Task RenderText();
	void SetupSprites();
	void UpdateSprites();
};

}