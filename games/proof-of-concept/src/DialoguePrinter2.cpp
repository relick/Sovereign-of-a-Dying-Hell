#include "DialoguePrinter2.hpp"
#include "Constants.hpp"

#include <genesis.h>

#include <cstring>

namespace Game
{

inline constexpr u16 c_lineCount = 3;
inline constexpr u16 c_lineWidth = 36;
inline constexpr u16 c_pixelsPerTile = 8;

//------------------------------------------------------------------------------
void DialoguePrinter2::Init(TileSet const &i_font)
{
	// Queue cleared tiles
	VDP_fillTileData(0, 1536 - m_tiles.size(), m_tiles.size(), true);

	// Tilemap indices setup as
	// 108-XX-117                118-XX-127
	// 0-XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX-107

	u16 namesIndex = (c_lineCount * c_lineWidth) + (1536 - m_tiles.size());
	u16 textIndex = 0 + (1536 - m_tiles.size());

	for(u16 y = 0; y < 32; ++y)
	{
		if(y >= c_textFramePos && y <= (c_textFramePos + 3))
		{
			if(y == c_textFramePos)
			{
				for (u16 x = 0; x < 10; ++x)
				{
					m_tileMap[2 + x + y * 64] = TILE_ATTR_FULL(PAL3, 0, 0, 0, namesIndex++);
				}

				for (u16 x = 0; x < 10; ++x)
				{
					m_tileMap[2 + 26 + x + y * 64] = TILE_ATTR_FULL(PAL3, 0, 0, 0, namesIndex++);
				}
			}
			else
			{
				for (u16 x = 0; x < c_lineWidth; ++x)
				{
					m_tileMap[2 + x + y * 64] = TILE_ATTR_FULL(PAL3, 0, 0, 0, textIndex++);
				}
			}
		}
	}

	VDP_setTileMapData(VDP_WINDOW, m_tileMap.data(), 0, m_tileMap.size(), 2, DMA_QUEUE);

	// Parse font for widths up front
	m_font = &i_font;
	u16 srcIndex = 0;
	for(u16 i = 0; i < m_fontData.size(); ++i)
	{
		m_fontData[i].m_srcIndex = srcIndex;
		m_fontData[i].m_charWidth = 0;
		for (u16 t = 0; t < 8; ++t, ++srcIndex)
		{
			u32 row = m_font->tiles[srcIndex];
			s8 width = 8;
			while((row & 0xF) == 0 && width > 0)
			{
				--width;
				row >>= 4;
			}
			if(m_fontData[i].m_charWidth < width)
			{
				m_fontData[i].m_charWidth = width;
			}
		}
	}


}

//------------------------------------------------------------------------------
void DialoguePrinter2::SetText
(
	char const* i_text
)
{
	m_curText = i_text;
	m_curTextLen = std::strlen(i_text);
	m_curTextIndex = 0;
	m_lastCharWasSpace = true;
	m_x = 0;
	m_y = 0;
}

//------------------------------------------------------------------------------
bool DialoguePrinter2::Update()
{
	if (m_curText && m_curTextIndex < m_curTextLen)
	{
		bool const noMoreThisDisp = DrawChar();
		QueueDMA();
		return noMoreThisDisp && m_curTextIndex == m_curTextLen;
	}
	return true;
}

//------------------------------------------------------------------------------
void DialoguePrinter2::Next()
{
	if(!m_curText)
	{
		return;
	}

	if(!DrawChar())
	{
		if (m_curTextIndex == m_curTextLen)
		{
			// End line
			m_curText = nullptr;
			return;
		}

		// Can't print any more already, so move to next
		std::fill(m_tiles.begin(), m_tiles.begin() + c_lineWidth * c_lineCount, Tile{});
		m_x = 0;
		m_y = 0;
		m_lastCharWasSpace = true;
	}
	else
	{
		// Could print more so print to end of current display
		while(DrawChar())
		{
		}
	}

	QueueDMA();
}

//------------------------------------------------------------------------------
bool DialoguePrinter2::DrawChar
(
)
{
	if (m_y >= c_lineCount || m_curTextIndex == m_curTextLen)
	{
		return false; // Need to progress or we're at the end of the line
	}

	// Main logic for stepping through and deciding how our display progresses
	if (m_curTextIndex == 0)
	{
		std::fill(m_tiles.begin(), m_tiles.begin() + c_lineWidth * c_lineCount, Tile{});
	}

	// Word wrapping handling, run on first character of each word
	if(m_lastCharWasSpace)
	{
		char const* word = m_curText + m_curTextIndex;
		u16 wordLen = 0;
		while (*word != ' ' && *word != '\n' && *word != '\0')
		{
			wordLen += m_fontData[*word - 32].m_charWidth;
			++word;
		}

		if(m_x + wordLen >= c_lineWidth * c_pixelsPerTile)
		{
			if(m_y + 1 >= c_lineCount)
			{
				return false;
			}
			else
			{
				m_x = 0;
				m_y++;
			}
		}
	}

	u8 curChar = m_curText[m_curTextIndex];

	m_lastCharWasSpace = curChar == ' ';

	if (curChar == '\n')
	{
		m_lastCharWasSpace = true;
		m_x = 0;
		m_y++;
		m_curTextIndex++;
		return m_y < c_lineCount;
	}

	u8 charFontDataI = curChar - 32;

	// Blit!
	if (curChar != ' ')
	{
		// Whilst we have 8 pixel tall text that aligns with the tiles, a character spans 2 tiles generally. So we'll do left tile, then right tile
		u16 const tileInd = (m_y * c_lineWidth) + (m_x >> 3);
		u32 *const leftTile = (u32 *)&(m_tiles[tileInd].m_pixels);

		// So for the left tile, we need to just shift the font data to the right by the amount of pixels m_x is in to the tile
		u8 leftTilePixels = m_x & 0x7;
		u16 const shift = leftTilePixels << 2;
		for (u8 i = 0; i < 8; ++i)
		{
			leftTile[i] |= m_font->tiles[m_fontData[charFontDataI].m_srcIndex + i] >> shift;
		}

		u16 const used = c_pixelsPerTile - leftTilePixels;
		if (used < m_fontData[charFontDataI].m_charWidth)
		{
			u32 *const rightTile = (u32 *)&(m_tiles[tileInd + 1].m_pixels);
			u16 antiShift = used << 2;
			for(u8 i = 0; i < 8; ++i)
			{
				rightTile[i] |= m_font->tiles[m_fontData[charFontDataI].m_srcIndex + i] << antiShift;
			}
		}

		m_x += m_fontData[charFontDataI].m_charWidth;
	}
	else
	{
		// Default space size
		m_x += 4;
	}

	// Update counters
	m_curTextIndex++;

	if(m_x >= c_lineWidth * c_pixelsPerTile)
	{
		m_x = 0;
		m_y++;
		if(m_y >= c_lineCount)
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
void DialoguePrinter2::QueueDMA()
{
	VDP_loadTileData((u32 *)m_tiles.data(), 1536 - m_tiles.size(), m_tiles.size(), DMA_QUEUE);
}

}