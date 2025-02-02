#include "DialoguePrinter2.hpp"

#include "Constants.hpp"
#include "Debug.hpp"
#include "FontData.hpp"
#include "Game.hpp"
#include "SpriteManager.hpp"

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

inline constexpr u16 c_namePosSide = 10;
inline constexpr s16 c_namePosDown = -10;

inline constexpr u16 c_textPosSide = 24;
inline constexpr s16 c_textPosDown = -5;
inline constexpr u16 c_lineSeparation = 2;
inline constexpr u16 c_lineIndent = 1;

inline constexpr u8 c_arrowSpeed = 2; // Number of updates between arrow flashes i.e. higher number = longer.

//------------------------------------------------------------------------------
DialoguePrinter2::DialoguePrinter2
(
	Game& io_game,
	FontData const& i_fonts
)
	: m_game{ &io_game }
	, m_fonts{ &i_fonts }
{
	m_dmaCallbackID = m_game->AddVBlankCallback(
		[this] {
			// Wait for next frame if there are any tasks queued, to avoid piling too much DMA
			// Text can be quite heavy!
			if (m_game->TasksInProgress())
			{
				return;
			}

			// Must clear VRAM by DMA fill in the VBlank, otherwise it swallows up the whole frame
			if (!m_vramInitialised)
			{
				VDP_fillTileData(0, c_textTilesIndex, m_tiles.size(), true);
				m_vramInitialised = true;
				return;
			}

			if(m_nameTileRefresh)
			{
				DMA_doDmaFast(
					DMA_VRAM,
					m_tiles.data() + c_textTileCount,
					c_nameTilesAddress,
					c_nameTileCount * (sizeof(Tiles::Tile) >> 1),
					2
				);

				m_nameTileRefresh = false;
			}
			
			if(m_lineTileRefreshStart <= m_lineTileRefreshEnd)
			{
				DMA_doDmaFast(
					DMA_VRAM,
					m_tiles.data() + m_lineTileRefreshStart,
					c_textTilesAddress + (sizeof(Tiles::Tile) * m_lineTileRefreshStart),
					(1 + m_lineTileRefreshEnd - m_lineTileRefreshStart) * (sizeof(Tiles::Tile) >> 1),
					2
				);

				m_lineTileRefreshStart = UINT16_MAX;
				m_lineTileRefreshEnd = 0;
			}
		}
	);
}

//------------------------------------------------------------------------------
DialoguePrinter2::~DialoguePrinter2
(
)
{
	// Remove vblank and sprites
	m_game->RemoveVBlankCallback(m_dmaCallbackID);

	if (m_spritesInitialised)
	{
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
		auto [id, spr] = m_game->Sprites().AddSprite(
			(i == (m_nameSprites.size() - 1)) ? SpriteSize::r2c1 : SpriteSize::r2c4,
			TILE_ATTR_FULL(PAL3, true, false, false, nameIndex)
		);

		spr.SetX(c_namePosSide + i * 32);
		spr.SetY(c_namePosDown + (c_textFramePos - 1) * 8);
		spr.SetZ(z++);

		m_nameSprites[i] = id;

		nameIndex += 8;
	}

	// Text sprites
	u16 textIndex = c_textTilesIndex;
	u16 sprI = 0;
	for (u16 y = 0; y < c_lineCount;)
	{
		for (u16 x = 0; x < c_lineWidth;)
		{
			auto [id, spr] = m_game->Sprites().AddSprite(
				((c_lineWidth - x) < 4) ? SpriteSize::r1c2 : SpriteSize::r1c4,
				TILE_ATTR_FULL(PAL3, true, false, false, textIndex)
			);

			spr.SetX(c_textPosSide + x * 8 + y * c_lineIndent);
			spr.SetY(c_textPosDown + (c_textFramePos + 1 + y) * 8 + (c_lineSeparation * y));
			spr.SetZ(z++);

			textIndex += ((c_lineWidth - x) < 4) ? 2 : 4;

			m_textSprites[sprI] = id;

			++sprI;
			x += 4;
		}

		++y;
	}

	// Arrow sprite
	{
		auto [id, spr] = m_game->Sprites().AddSprite(
			SpriteSize::r1c1,
			TILE_ATTR_FULL(PAL3, true, false, false, m_game->Sprites().InsertMiscTiles(misc_spr))
		);

		spr.SetVisible(false);
		spr.SetX(304);
		spr.SetY(c_textPosDown + (c_textFramePos + 1 + 2) * 8 + (c_lineSeparation * 2) + 4);
		
		m_nextArrow = id;
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
	std::fill(m_tiles.begin() + c_textTileCount, m_tiles.end(), Tiles::Tile{});

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
			auto const [upperTile, lowerTile] = m_fonts->GetVNNameFontTiles(curChar);

			m_tiles[c_textTileCount + (limit++)] = *upperTile;
			m_tiles[c_textTileCount + limit] = *lowerTile;
		}

		++i_name;
	}

	// Update sprites
	if (m_nameOnLeft != i_left)
	{
		for (u16 i = 0; i < m_nameSprites.size(); ++i)
		{
			s16 x = c_namePosSide + i * 32;
			if(!i_left)
			{
				x += 320 - 2 * c_namePosSide - ((limit >> 1) * 8);
			}

			m_game->Sprites().EditSpriteData(m_nameSprites[i]).SetX(x);
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
	m_doneAllText = false;
	m_curText = i_text;
	m_curTextLen = std::strlen(i_text);
	m_curTextIndex = 0;
	m_lastCharWasSpace = true;
	m_x = 0;
	m_y = 0;
}

//------------------------------------------------------------------------------
void DialoguePrinter2::Update()
{
	if (!m_spritesInitialised)
	{
		if (m_vramInitialised)
		{
			SetupSprites();
			m_spritesInitialised = true;
		}
	}

	// TODO: properly controllable timer
	static u16 time = 0;
	if (time == 3)
	{
		time = 0;
	}
	else
	{
		++time;
		return;
	}

	bool showArrow = true;
	if (m_curText && m_curTextIndex < m_curTextLen)
	{
		bool const moreToDisplay = DrawChar();
		showArrow = !moreToDisplay; // && m_curTextIndex != m_curTextLen; // Always display the arrow when at the end now
		m_doneAllText = !moreToDisplay && m_curTextIndex == m_curTextLen;
	}
	else
	{
		m_doneAllText = true;
	}

	if (showArrow)
	{
		++m_arrowTimer;
		if (m_arrowTimer >= c_arrowSpeed)
		{
			EditableSpriteData arrowSpr = m_game->Sprites().EditSpriteData(m_nextArrow);
			arrowSpr.SetVisible(!arrowSpr.IsVisible());
			m_arrowTimer = 0;
		}
	}

}

//------------------------------------------------------------------------------
bool DialoguePrinter2::Done() const
{
	return m_doneAllText;
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
			m_game->Sprites().EditSpriteData(m_nextArrow).SetVisible(false);
		}

		if (m_curTextIndex == m_curTextLen)
		{
			// End line
			m_curText = nullptr;
			return;
		}

		// Can't print any more already, so move to next
		std::fill(m_tiles.begin(), m_tiles.begin() + c_textTileCount, Tiles::Tile{});
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
		std::fill(m_tiles.begin(), m_tiles.begin() + c_textTileCount, Tiles::Tile{});
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
			wordLen += m_fonts->GetVNTextFontCharWidth(*word++);
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

	// Blit!
	if (curChar != ' ')
	{
		u32 const* curCharTileRows = m_fonts->GetVNTextFontTile(curChar)->AsRawRows();
		u8 const curCharWidth = m_fonts->GetVNTextFontCharWidth(curChar);

		// Whilst we have 8 pixel tall text that aligns with the tiles, a character spans 2 tiles generally. So we'll do left tile, then right tile
		u16 const tileInd = (m_y * c_lineWidth) + (m_x >> 3);
		u32* const leftTile = m_tiles[tileInd].AsRawRows();


		// So for the left tile, we need to just shift the font data to the right by the amount of pixels m_x is in to the tile
		u8 leftTilePixels = m_x & 0x7;
		u16 const shift = leftTilePixels << 2;
		for (u8 i = 0; i < 8; ++i)
		{
			leftTile[i] |= curCharTileRows[i] >> shift;
		}
		QueueTextDMA(tileInd);

		u16 const used = c_pixelsPerTile - leftTilePixels;
		if (used < curCharWidth)
		{
			u32* const rightTile = m_tiles[tileInd + 1].AsRawRows();
			u16 antiShift = used << 2;
			for(u8 i = 0; i < 8; ++i)
			{
				rightTile[i] |= curCharTileRows[i] << antiShift;
			}
			QueueTextDMA(tileInd + 1);
		}

		m_x += curCharWidth;
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
void DialoguePrinter2::QueueTextDMA
(
	u16 i_tileIndex
)
{
	m_lineTileRefreshStart = std::min(i_tileIndex, m_lineTileRefreshStart);
	m_lineTileRefreshEnd = std::max(i_tileIndex, m_lineTileRefreshEnd);
}

}