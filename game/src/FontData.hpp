#pragma once

#include "Declare.hpp"
#include "TileData.hpp"

#include <array>

namespace Game
{

inline constexpr u16 c_vnNameFontCharaCount = 26 /* upper alphabet */ + 1 /* question mark */ + 1 /* space */;
inline constexpr u16 c_vnNameFontLowerTileOffset = c_vnNameFontCharaCount;

//------------------------------------------------------------------------------
/// Preprocesses and stores data about fonts for use by text rendering systems
//------------------------------------------------------------------------------
class FontData
{
	struct VariableWidthCharData
	{
		u8 m_charWidth{8};
	};
	using VNFontAuxData = std::array<VariableWidthCharData, 96>;

	TileSet const* m_vnTextFont{nullptr};
	VNFontAuxData m_vnTextFontAuxData{};
	TileSet const* m_vnDescFont{ nullptr };
	VNFontAuxData m_vnDescFontAuxData{};

	TileSet const* m_vnNameFont{nullptr};

public:
	// Processes fonts
	void Init(
		TileSet const& i_vnTextFont,
		TileSet const& i_vnDescFont,
		TileSet const& i_vnNameFont
	);

	Tiles::Tile const* GetVNTextFontTile(char i_asciiChar) const { return Tiles::AsTiles(m_vnTextFont->tiles) + (i_asciiChar - 32); }
	u8 GetVNTextFontCharWidth(char i_asciiChar) const { return m_vnTextFontAuxData[i_asciiChar - 32].m_charWidth; }
	Tiles::Tile const* GetVNDescFontTile(char i_asciiChar) const { return Tiles::AsTiles(m_vnDescFont->tiles) + (i_asciiChar - 32); }
	u8 GetVNDescFontCharWidth(char i_asciiChar) const { return m_vnDescFontAuxData[i_asciiChar - 32].m_charWidth; }
	
	// Returns upper and lower tile
	std::pair<Tiles::Tile const*, Tiles::Tile const*> GetVNNameFontTiles(char i_asciiChar) const {
		if (i_asciiChar == '?')
		{
			Tiles::Tile const* upper = Tiles::AsTiles(m_vnNameFont->tiles) + 26;
			return { upper, upper + c_vnNameFontLowerTileOffset, };
		}
		else if (i_asciiChar == ' ')
		{
			Tiles::Tile const* upper = Tiles::AsTiles(m_vnNameFont->tiles) + 27;
			return { upper, upper + c_vnNameFontLowerTileOffset, };
		}
		Tiles::Tile const* upper = Tiles::AsTiles(m_vnNameFont->tiles) + (i_asciiChar - 'A');
		return { upper, upper + c_vnNameFontLowerTileOffset, };
	}
};

}