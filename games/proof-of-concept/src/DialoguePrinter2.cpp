#include "DialoguePrinter2.hpp"
#include "Constants.hpp"
#include "Game.hpp"
#include "SpriteManager.hpp"

#include <genesis.h>

#include <cstring>

namespace Game
{

inline constexpr u16 c_lineCount = 3;
inline constexpr u16 c_lineWidth = 34;
inline constexpr u16 c_pixelsPerTile = 8;

inline constexpr u16 c_textTilesLoc = 0xD000 >> 5;

inline constexpr u16 c_nameFontOffset = 26 * 8;
inline constexpr u16 c_namePosSide = 10;
inline constexpr u16 c_namePosUp = 5;

inline constexpr u16 c_textPosSide = 24;
inline constexpr u16 c_textPosDown = 0;
inline constexpr u16 c_lineSeparation = 2;
inline constexpr u16 c_lineIndent = 1;

//------------------------------------------------------------------------------
void DialoguePrinter2::Init
(
	Game& io_game,
	TileSet const &i_textFont,
	TileSet const &i_nameFont
)
{
	// Queue cleared tiles
	VDP_fillTileData(0, c_textTilesLoc, m_tiles.size(), true);

	// Parse font for widths up front
	m_textFont = &i_textFont;
	u16 srcIndex = 0;
	for(u16 i = 0; i < m_textFontData.size(); ++i)
	{
		m_textFontData[i].m_srcIndex = srcIndex;
		m_textFontData[i].m_charWidth = 0;
		for (u16 t = 0; t < 8; ++t, ++srcIndex)
		{
			u32 row = m_textFont->tiles[srcIndex];
			s8 width = 8;
			while((row & 0xF) == 0 && width > 0)
			{
				--width;
				row >>= 4;
			}
			if (m_textFontData[i].m_charWidth < width)
			{
				m_textFontData[i].m_charWidth = width;
			}
		}
	}

	m_nameFont = &i_nameFont;
	for(u16 i = 0; i < m_nameFontData.size(); ++i)
	{
		m_nameFontData[i] = i * 8;
	}

	SetupSprites(io_game);
}

//------------------------------------------------------------------------------
void DialoguePrinter2::SetupSprites
(
	Game& io_game
)
{
	// Tile indices setup as
	// 102-XXXXXXXXX
	// XXXXXXXXX-127
	// 0-XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX-101

	// Name sprites
	u16 nameIndex = (c_lineCount * c_lineWidth) + c_textTilesLoc;
	for (u16 i = 0; i < m_sprites.m_nameSprites.size(); ++i)
	{
		SpriteData spr;

		spr.m_x = c_namePosSide + i * 32;
		spr.m_y = -c_namePosUp + (c_textFramePos - 1) * 8;
		spr.m_palette = SpritePalette::Pal3;
		spr.m_highPriority = true;
		spr.m_firstTileIndex = nameIndex;

		if (i == (m_sprites.m_nameSprites.size() - 1))
		{
			spr.m_size = SpriteSize::r2c1;
		}
		else
		{
			spr.m_size = SpriteSize::r2c4;
		}
		
		m_sprites.m_nameSprites[i] = io_game.Sprites().AddSprite(std::move(spr)).first;

		nameIndex += 8;
	}

	// Text sprites
	u16 textIndex = c_textTilesLoc;
	u16 sprI = 0;
	for (u16 y = 0; y < c_lineCount;)
	{
		for (u16 x = 0; x < c_lineWidth;)
		{
			SpriteData spr;

			spr.m_x = c_textPosSide + x * 8 + y * c_lineIndent;
			spr.m_y = c_textPosDown + (c_textFramePos + 1 + y) * 8 + (c_lineSeparation * y);
			spr.m_palette = SpritePalette::Pal3;
			spr.m_highPriority = true;
			spr.m_firstTileIndex = textIndex;
			
			if ((c_lineWidth - x) < 4)
			{
				spr.m_size = SpriteSize::r1c2;
				textIndex += 2;
			}
			else
			{
				spr.m_size = SpriteSize::r1c4;
				textIndex += 4;
			}

			m_sprites.m_textSprites[sprI] = io_game.Sprites().AddSprite(std::move(spr)).first;

			++sprI;
			x += 4;
		}

		++y;
	}
}

//------------------------------------------------------------------------------
void DialoguePrinter2::SetName
(
	Game& io_game,
	char const* i_name,
	bool i_left
)
{
	// Clear current name
	std::fill(m_tiles.begin() + (c_lineWidth * c_lineCount), m_tiles.end(), Tile{});

	// Fill new name
	u8 limit = 0;
	for (; limit < 26; ++limit)
	{
		char const curChar = *i_name;
		if (curChar == '\0')
		{
			break; // done
		}

		if (curChar >= 'A' && curChar <= 'Z')
		{
			u16 const srcIndexUpper = m_nameFontData[curChar - 'A'];
			u16 const srcIndexLower = srcIndexUpper + c_nameFontOffset;
			std::memcpy(&(m_tiles[(c_lineWidth * c_lineCount) + (limit++)]), m_nameFont->tiles + srcIndexUpper, sizeof(Tile));
			std::memcpy(&(m_tiles[(c_lineWidth * c_lineCount) + limit]), m_nameFont->tiles + srcIndexLower, sizeof(Tile));
		}

		++i_name;
	}

	QueueDMA();

	// Update sprites
	if (m_nameOnLeft != i_left)
	{
		for (u16 i = 0; i < m_sprites.m_nameSprites.size(); ++i)
		{
			SpriteData& spr = io_game.Sprites().EditSpriteData(m_sprites.m_nameSprites[i]);
			spr.m_x = c_namePosSide + i * 32;
			if(!i_left)
			{
				spr.m_x += 320 - 2 * c_namePosSide - ((limit >> 1) * 8);
			}
		}

		m_nameOnLeft = i_left;
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
		std::fill(m_tiles.begin(), m_tiles.begin() + (c_lineWidth * c_lineCount), Tile{});
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
			wordLen += m_textFontData[*word - 32].m_charWidth;
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
			leftTile[i] |= m_textFont->tiles[m_textFontData[charFontDataI].m_srcIndex + i] >> shift;
		}

		u16 const used = c_pixelsPerTile - leftTilePixels;
		if (used < m_textFontData[charFontDataI].m_charWidth)
		{
			u32 *const rightTile = (u32 *)&(m_tiles[tileInd + 1].m_pixels);
			u16 antiShift = used << 2;
			for(u8 i = 0; i < 8; ++i)
			{
				rightTile[i] |= m_textFont->tiles[m_textFontData[charFontDataI].m_srcIndex + i] << antiShift;
			}
		}

		m_x += m_textFontData[charFontDataI].m_charWidth;
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
	// TODO: at least split by whether name or text is being uploaded, and also attempt to optimise text where possible
	DMA_queueDmaFast(DMA_VRAM, m_tiles.data(), c_textTilesLoc << 5, sizeof(m_tiles) >> 1, 2);
}

}