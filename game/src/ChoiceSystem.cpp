#include "ChoiceSystem.hpp"

#include "FontData.hpp"
#include "Game.hpp"
#include "Version.hpp"

#include "res_vn.h"

namespace Game
{

inline constexpr u16 c_textTilesAddress = 0xD000;
inline constexpr u16 c_textTilesIndex = c_textTilesAddress / 32;

inline constexpr u16 c_pixelsPerTile = 8;
inline constexpr u16 c_spritesPerChoice = 7;
inline constexpr u16 c_tilesPerChoice = c_spritesPerChoice * 4;
inline constexpr u16 c_pixelsPerChoice = c_tilesPerChoice * c_pixelsPerTile;

inline constexpr s16 c_choiceXPos = (c_screenWidthPx - c_pixelsPerChoice) / 2;
inline constexpr s16 c_choiceYSeparation = 16;
inline constexpr s16 c_choiceArrowXOffset = -12;

//------------------------------------------------------------------------------
ChoiceSystem::ChoiceSystem
(
	Game& io_game,
	FontData const& i_fonts
)
	: m_game{ &io_game }
	, m_fonts{ &i_fonts }
{
}

//------------------------------------------------------------------------------
ChoiceSystem::~ChoiceSystem()
{
	for (SpriteID id : m_choiceTextSprites)
	{
		m_game->Sprites().RemoveSprite(id);
	}

	m_game->Sprites().RemoveSprite(m_choiceArrow);
}

//------------------------------------------------------------------------------
void ChoiceSystem::SetChoices
(
	std::span<char const* const> i_choices,
	std::optional<f16> i_timeLimitInSeconds // = std::nullopt
)
{
	m_choices = i_choices;
	m_baseChoiceY = (c_screenHeightPx / 2) - ((m_choices.size() / 2) * c_choiceYSeparation);
	m_timeLimit = i_timeLimitInSeconds;

	// Text rendering
	m_game->QueueFunctionTask(RenderText());
	m_game->QueueLambdaTask([this] -> Task { SetupSprites(); co_return; });
}

//------------------------------------------------------------------------------
std::expected<u8, ChoiceSystem::NoChoiceMade> ChoiceSystem::Update()
{
	if (m_timeLimit)
	{
		*m_timeLimit -= FrameStep();
		if (*m_timeLimit < 0)
		{
			return std::unexpected(NoChoiceMade::TimeLimitReached);
		}
	}

	if (m_game->TasksInProgress())
	{
		return std::unexpected(NoChoiceMade::Waiting);
	}

	u16 const buttons = JOY_readJoypad(JOY_1);
	static bool upPressedPrev = false;
	static bool downPressedPrev = false;
	static bool choosePressedPrev = false;

	bool upPressed = false;
	bool downPressed = false;
	bool choosePressed = false;

	if ((buttons & BUTTON_UP) != 0)
	{
		if (!upPressedPrev)
		{
			upPressed = true;
		}
		upPressedPrev = true;
	}
	else
	{
		upPressedPrev = false;
	}

	if ((buttons & BUTTON_DOWN) != 0)
	{
		if (!downPressedPrev)
		{
			downPressed = true;
		}
		downPressedPrev = true;
	}
	else
	{
		downPressedPrev = false;
	}

	if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
	{
		if (!choosePressedPrev)
		{
			choosePressed = true;
		}
		choosePressedPrev = true;
	}
	else
	{
		choosePressedPrev = false;
	}

	if (upPressed)
	{
		if (m_highlightedChoice > 0)
		{
			--m_highlightedChoice;
		}
		else
		{
			m_highlightedChoice = m_choices.size() - 1;
		}
	}

	if (downPressed)
	{
		if (m_highlightedChoice < m_choices.size() - 1)
		{
			++m_highlightedChoice;
		}
		else
		{
			m_highlightedChoice = 0;
		}
	}

	if (choosePressed)
	{
		return m_highlightedChoice;
	}

	UpdateSprites();

	return std::unexpected(NoChoiceMade::Waiting);
}

//------------------------------------------------------------------------------
Task ChoiceSystem::RenderText()
{
	u16 strIndex = 0;
	for (char const* str : m_choices)
	{
		u16 const baseTileIndex = strIndex * c_tilesPerChoice;
		u16 x = 0;
		while (*str != '\0')
		{
			char const curChar = *str;
			if (curChar != ' ')
			{
				u8 const curCharWidth = m_fonts->GetVNTextFontCharWidth(curChar);
				if (curCharWidth + x > c_pixelsPerChoice)
				{
					// Can't fit any more characters
					++str;
					continue;
				}

				u32 const* curCharTileRows = m_fonts->GetVNTextFontTile(curChar)->AsRawRows();

				// Whilst we have 8 pixel tall text that aligns with the tiles, a character spans 2 tiles generally. So we'll do left tile, then right tile
				u16 const tileInd = baseTileIndex + (x >> 3);
				u32* const leftTile = m_tiles[tileInd].AsRawRows();

				// So for the left tile, we need to just shift the font data to the right by the amount of pixels m_x is in to the tile
				u8 leftTilePixels = x & 0x7;
				u16 const shift = leftTilePixels << 2;
				for (u8 i = 0; i < 8; ++i)
				{
					leftTile[i] |= curCharTileRows[i] >> shift;
				}

				u16 const used = c_pixelsPerTile - leftTilePixels;
				if (used < curCharWidth)
				{
					u32* const rightTile = m_tiles[tileInd + 1].AsRawRows();
					u16 antiShift = used << 2;
					for (u8 i = 0; i < 8; ++i)
					{
						rightTile[i] |= curCharTileRows[i] << antiShift;
					}
				}

				x += curCharWidth;
			}
			else if (x > 0)
			{
				// Default space size, but only if we're not at the start of a line
				x += 4;
			}
			++str;
		}

		while (!DMA_queueDmaFast(
			DMA_VRAM,
			m_tiles.data() + baseTileIndex,
			c_textTilesAddress + (sizeof(Tiles::Tile) * baseTileIndex),
			c_tilesPerChoice * (sizeof(Tiles::Tile) >> 1),
			2
		))
		{
			co_yield{};
		}
		co_yield{};

		++strIndex;
	}

	co_return;
}

//------------------------------------------------------------------------------
void ChoiceSystem::SetupSprites
(
)
{
	s8 z = -128;
	u16 tileIndex = c_textTilesIndex;
	for (u16 i = 0; i < m_choices.size(); ++i)
	{
		s16 const separation = i * c_choiceYSeparation;
		for (u16 sprN = 0; sprN < c_spritesPerChoice; ++sprN)
		{
			auto [id, spr] = m_game->Sprites().AddSprite(
				SpriteSize::r1c4,
				TILE_ATTR_FULL(PAL3, true, false, false, tileIndex + (4 * sprN))
			);

			spr.SetX(c_choiceXPos + (4 * c_pixelsPerTile * sprN));
			spr.SetY(m_baseChoiceY + separation);
			spr.SetZ(z++);

			m_choiceTextSprites.push_back(id);
		}

		tileIndex += c_tilesPerChoice;
	}

	// Arrow sprite
	{
		auto [id, spr] = m_game->Sprites().AddSprite(
			SpriteSize::r1c1,
			TILE_ATTR_FULL(PAL3, true, false, false, m_game->Sprites().InsertMiscTiles(misc_spr) + 1) // 1 is where the right-arrow is
		);

		spr.SetVisible(false);
		spr.SetX(c_choiceXPos + c_choiceArrowXOffset);

		m_choiceArrow = id;
	}
}

//------------------------------------------------------------------------------
void ChoiceSystem::UpdateSprites
(
)
{
	m_floatScroll += 16;
	m_float = sinFix16(m_floatScroll) >> 4;

	// Arrow
	{
		auto spr = m_game->Sprites().EditSpriteData(m_choiceArrow);

		spr.SetY(m_baseChoiceY + (m_highlightedChoice * c_choiceYSeparation) + m_float);

		// TODO proper timer
		static u8 arrowTimer = 0;
		++arrowTimer;
		if (arrowTimer > 6)
		{
			spr.SetVisible(!spr.IsVisible());
			arrowTimer = 0;
		}
	}

	u16 sprI = 0;
	for (u16 i = 0; i < m_choices.size(); ++i)
	{
		s16 const separation = i * c_choiceYSeparation;
		s16 const choiceX = c_choiceXPos + (sinFix16((m_floatScroll + i * 36) * 4) >> 5);
		for (u16 sprN = 0; sprN < c_spritesPerChoice; ++sprN)
		{
			auto spr = m_game->Sprites().EditSpriteData(m_choiceTextSprites[sprI]);

			spr.SetY(m_baseChoiceY + separation + m_float);
			spr.SetX(choiceX + (4 * c_pixelsPerTile * sprN));

			++sprI;
		}
	}
}

}