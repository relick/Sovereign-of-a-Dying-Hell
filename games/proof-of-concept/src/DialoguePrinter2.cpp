#include "DialoguePrinter2.hpp"
#include "Constants.hpp"
#include "Game.hpp"
#include "SpriteManager.hpp"
#include "Debug.hpp"

#include <genesis.h>
#include "res_spr.h"

#include <cstring>

namespace Game
{

inline constexpr u16 c_lineCount = 3;
inline constexpr u16 c_lineWidth = 34;
inline constexpr u16 c_pixelsPerTile = 8;

inline constexpr u16 c_textTileCount = c_lineCount * c_lineWidth;
inline constexpr u16 c_nameTileCount = 128 - c_textTileCount;

inline constexpr u16 c_textTilesAddress = 0xD000;
inline constexpr u16 c_textTilesIndex = c_textTilesAddress / 32;
inline constexpr u16 c_nameTilesIndex = c_textTilesIndex + c_textTileCount;
inline constexpr u16 c_nameTilesAddress = c_nameTilesIndex * 32;

inline constexpr u16 c_nameFontOffset = 26 * 8;
inline constexpr u16 c_namePosSide = 10;
inline constexpr s16 c_namePosDown = -9;

inline constexpr u16 c_textPosSide = 24;
inline constexpr s16 c_textPosDown = -4;
inline constexpr u16 c_lineSeparation = 2;
inline constexpr u16 c_lineIndent = 1;

inline constexpr u8 c_arrowSpeed = 2; // Number of updates between arrow flashes i.e. higher number = longer.

//------------------------------------------------------------------------------
void DialoguePrinter2::Init
(
	Game& io_game,
	TileSet const &i_textFont,
	TileSet const &i_nameFont
)
{
	m_game = &io_game;

	// Queue cleared tiles
	VDP_fillTileData(0, c_textTilesIndex, m_tiles.size(), true);

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

	SetupSprites();

	m_dmaCallbackID = m_game->AddVBlankCallback(
		[this]{
			if(m_nameTileRefresh)
			{
				DMA_doDmaFast(
					DMA_VRAM,
					m_tiles.data() + c_textTileCount,
					c_nameTilesAddress,
					c_nameTileCount * (sizeof(Tile) >> 1),
					2
				);

				m_nameTileRefresh = false;
			}
			
			if(m_lineTileRefreshStart <= m_lineTileRefreshEnd)
			{
				DMA_doDmaFast(
					DMA_VRAM,
					m_tiles.data() + m_lineTileRefreshStart,
					c_textTilesAddress + (sizeof(Tile) * m_lineTileRefreshStart),
					(1 + m_lineTileRefreshEnd - m_lineTileRefreshStart) * (sizeof(Tile) >> 1),
					2
				);

				m_lineTileRefreshStart = UINT16_MAX;
				m_lineTileRefreshEnd = 0;
			}
		}
	);
}

//------------------------------------------------------------------------------
void DialoguePrinter2::Shutdown
(
)
{
	// Remove vblank and sprites
	m_game->RemoveVBlankCallback(m_dmaCallbackID);

	// Should clean up even if the world will do it, in case we spawn and kill multiple DialoguePrinters!
	for(SpriteID id : m_nameSprites)
	{
		m_game->Sprites().RemoveSprite(id);
	}
	for(SpriteID id : m_textSprites)
	{
		m_game->Sprites().RemoveSprite(id);
	}
	m_game->Sprites().RemoveSprite(m_nextArrow);
}

//------------------------------------------------------------------------------
void DialoguePrinter2::SetupSprites
(
)
{
	// Tile indices setup as
	// 102-XXXXXXXXX
	// XXXXXXXXX-127
	// 0-XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX-101

	// Name sprites
	s8 z = -128;

	u16 nameIndex = c_nameTilesIndex;
	for (u16 i = 0; i < m_nameSprites.size(); ++i)
	{
		SpriteData spr;

		spr.m_x = c_namePosSide + i * 32;
		spr.m_y = c_namePosDown + (c_textFramePos - 1) * 8;
		spr.m_palette = SpritePalette::Pal3;
		spr.m_highPriority = true;
		spr.m_firstTileIndex = nameIndex;
		spr.m_z = z++;

		if (i == (m_nameSprites.size() - 1))
		{
			spr.m_size = SpriteSize::r2c1;
		}
		else
		{
			spr.m_size = SpriteSize::r2c4;
		}
		
		m_nameSprites[i] = m_game->Sprites().AddSprite(std::move(spr)).first;

		nameIndex += 8;
	}

	// Text sprites
	u16 textIndex = c_textTilesIndex;
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
			spr.m_z = z++;
			
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

			m_textSprites[sprI] = m_game->Sprites().AddSprite(std::move(spr)).first;

			++sprI;
			x += 4;
		}

		++y;
	}

	// Arrow sprite
	{
		SpriteData spr;
		spr.m_firstTileIndex = m_game->Sprites().InsertMiscTiles(misc_spr);
		spr.m_size = SpriteSize::r1c1;
		spr.m_palette = SpritePalette::Pal3;
		spr.m_highPriority = true;
		spr.m_visible = false;
		spr.m_x = 304;
		spr.m_y = c_textPosDown + (c_textFramePos + 1 + 2) * 8 + (c_lineSeparation * 2) + 4;

		m_nextArrow = m_game->Sprites().AddSprite(std::move(spr)).first;
	}
}

//------------------------------------------------------------------------------
void DialoguePrinter2::SetName
(
	char const* i_name,
	bool i_left
)
{
	if (i_name == m_curName)
	{
		return;
	}

	m_curName = i_name;

	// Clear current name
	std::fill(m_tiles.begin() + c_textTileCount, m_tiles.end(), Tile{});

	QueueNameDMA();

	if (!i_name || std::strlen(i_name) == 0)
	{
		return;
	}

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
			std::memcpy(&(m_tiles[c_textTileCount + (limit++)]), m_nameFont->tiles + srcIndexUpper, sizeof(Tile));
			std::memcpy(&(m_tiles[c_textTileCount + limit]), m_nameFont->tiles + srcIndexLower, sizeof(Tile));
		}

		++i_name;
	}

	// Update sprites
	if (m_nameOnLeft != i_left)
	{
		for (u16 i = 0; i < m_nameSprites.size(); ++i)
		{
			SpriteData& spr = m_game->Sprites().EditSpriteData(m_nameSprites[i]);
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
		bool const moreToDisplay = DrawChar();

		if (!moreToDisplay && m_curTextIndex != m_curTextLen)
		{
			++m_arrowTimer;
			if (m_arrowTimer >= c_arrowSpeed)
			{
				SpriteData &arrowSpr = m_game->Sprites().EditSpriteData(m_nextArrow);
				arrowSpr.m_visible = !arrowSpr.m_visible;
				m_arrowTimer = 0;
			}
		}

		return !moreToDisplay && m_curTextIndex == m_curTextLen;
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
		{
			SpriteData &arrowSpr = m_game->Sprites().EditSpriteData(m_nextArrow);
			arrowSpr.m_visible = false;
		}

		if (m_curTextIndex == m_curTextLen)
		{
			// End line
			m_curText = nullptr;
			return;
		}

		// Can't print any more already, so move to next
		std::fill(m_tiles.begin(), m_tiles.begin() + c_textTileCount, Tile{});
		m_lineTileRefreshStart = 0;
		m_lineTileRefreshEnd = c_textTileCount;
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
		std::fill(m_tiles.begin(), m_tiles.begin() + c_textTileCount, Tile{});
		m_lineTileRefreshStart = 0;
		m_lineTileRefreshEnd = c_textTileCount;
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
			else if (m_x > 0)
			{
				m_x = 0;
				m_y++;
			}
			else
			{
				// Our word is too long for an *entire line*
				// So we just need to start displaying it and chop it into the next line.
				Error("Word was too long for entire line!");
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
		QueueCharacterDMA(tileInd);

		u16 const used = c_pixelsPerTile - leftTilePixels;
		if (used < m_textFontData[charFontDataI].m_charWidth)
		{
			u32 *const rightTile = (u32 *)&(m_tiles[tileInd + 1].m_pixels);
			u16 antiShift = used << 2;
			for(u8 i = 0; i < 8; ++i)
			{
				rightTile[i] |= m_textFont->tiles[m_textFontData[charFontDataI].m_srcIndex + i] << antiShift;
			}
			QueueCharacterDMA(tileInd + 1);
		}

		m_x += m_textFontData[charFontDataI].m_charWidth;
	}
	else if (m_x > 0)
	{
		// Default space size, but only if we're not at the start of a line
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
void DialoguePrinter2::QueueNameDMA()
{
	m_nameTileRefresh = true;
}

//------------------------------------------------------------------------------
void DialoguePrinter2::QueueCharacterDMA
(
	u16 i_charIndex
)
{
	m_lineTileRefreshStart = std::min(i_charIndex, m_lineTileRefreshStart);
	m_lineTileRefreshEnd = std::max(i_charIndex, m_lineTileRefreshEnd);
}

}