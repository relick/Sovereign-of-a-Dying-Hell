#pragma once

#include "Declare.hpp"
#include "TileData.hpp"

#include <array>
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

	VBlankCallbackID m_dmaCallbackID{};

	std::array<Tiles::Tile, 128> m_tiles{};

	// 4 sprites per choice i.e. 128 pixels
	std::vector<SpriteID> m_choiceTextSprites;
	SpriteID m_arrowSprite;

public:
	ChoiceSystem(Game& io_game, FontData const& i_fonts);
	~ChoiceSystem();

	void SetChoices(std::span<char const* const> i_choices);

	void Update();

private:
	void SetupSprites();

};

}