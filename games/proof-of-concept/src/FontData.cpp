#include "FontData.hpp"

namespace Game
{

//------------------------------------------------------------------------------
void FontData::Init
(
	TileSet const& i_vnTextFont,
	TileSet const& i_vnNameFont
)
{
	m_vnTextFont = &i_vnTextFont;
	m_vnNameFont = &i_vnNameFont;

	// Parse vn text font for characters widths
	Tiles::Tile const* vnTextTile = Tiles::AsTiles(m_vnTextFont->tiles);
	for(VariableWidthCharData& charData : m_vnTextFontAuxData)
	{
		charData.m_charWidth = 0;
		for (u8 rowI = 0; rowI < 8; ++rowI)
		{
			// TODO: special types for rows too?
			u32 row = vnTextTile->AsRawRows()[rowI];
			s8 width = 8;
			while((row & 0xF) == 0 && width > 0)
			{
				--width;
				row >>= 4;
			}
			if (charData.m_charWidth < width)
			{
				charData.m_charWidth = width;
			}
		}

		++vnTextTile;
	}
}

}