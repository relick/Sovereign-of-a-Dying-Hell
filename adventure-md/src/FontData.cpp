#include "adventure-md/FontData.hpp"

namespace Game
{

//------------------------------------------------------------------------------
void FontData::Init
(
	TileSet const& i_vnTextFont,
	TileSet const& i_vnDescFont,
	TileSet const& i_vnNameFont
)
{
	m_vnTextFont = &i_vnTextFont;
	m_vnDescFont = &i_vnDescFont;
	m_vnNameFont = &i_vnNameFont;

	auto fnCreateAuxDataForVNFont = [](TileSet const& i_font, VNFontAuxData& io_auxData)
	{
		// Parse vn text font for characters widths
		Tiles::Tile const* vnTextTile = Tiles::AsTiles(i_font.tiles);
		for (VariableWidthCharData& charData : io_auxData)
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
	};
	fnCreateAuxDataForVNFont(*m_vnTextFont, m_vnTextFontAuxData);
	fnCreateAuxDataForVNFont(*m_vnDescFont, m_vnDescFontAuxData);
}

}