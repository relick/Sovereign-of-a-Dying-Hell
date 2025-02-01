#pragma once

#include "Declare.hpp"
#include "TileData.hpp"

#include <genesis.h>

#include <array>
#include <string>

namespace Game
{

inline constexpr u16 c_vnNameFontLowerTileOffset = 26;

//------------------------------------------------------------------------------
/// Preprocesses and stores data about fonts for use by text rendering systems
//------------------------------------------------------------------------------
class FontData
{
	struct VariableWidthCharData
	{
		u8 m_charWidth{8};
	};

	TileSet const* m_vnTextFont{nullptr};
	std::array<VariableWidthCharData, 96> m_vnTextFontAuxData{};

	TileSet const* m_vnNameFont{nullptr};

public:
	// Processes fonts
	void Init(
		TileSet const& i_vnTextFont,
		TileSet const& i_vnNameFont
	);

	Tiles::Tile const* GetVNTextFontTile(char i_asciiChar) const { return Tiles::AsTiles(m_vnTextFont->tiles) + (i_asciiChar - 32); }
	u8 GetVNTextFontCharWidth(char i_asciiChar) const { return m_vnTextFontAuxData[i_asciiChar - 32].m_charWidth; }
	
	// Returns upper and lower tile
	std::pair<Tiles::Tile const*, Tiles::Tile const*> GetVNNameFontTiles(char i_asciiChar) const {
		Tiles::Tile const* upper = Tiles::AsTiles(m_vnNameFont->tiles) + (i_asciiChar - 'A');
		return { upper, upper + c_vnNameFontLowerTileOffset, };
	}
};

}