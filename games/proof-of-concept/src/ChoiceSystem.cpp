#include "ChoiceSystem.hpp"

#include "FontData.hpp"
#include "Game.hpp"

namespace Game
{

inline constexpr u16 c_textTilesAddress = 0xD000;
inline constexpr u16 c_textTilesIndex = c_textTilesAddress / 32;

inline constexpr u16 c_pixelsPerTile = 8;
inline constexpr u16 c_spritesPerChoice = 4;
inline constexpr u16 c_tilesPerChoice = c_spritesPerChoice * 4;
inline constexpr u16 c_pixelsPerChoice = c_tilesPerChoice * c_pixelsPerTile;

inline constexpr u16 c_choiceXPos = (c_screenWidthPx - c_pixelsPerChoice) / 2;

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
}

//------------------------------------------------------------------------------
void ChoiceSystem::SetChoices
(
	std::span<char const* const> i_choices
)
{
	m_choices = i_choices;

	// Text rendering
	m_game->QueueLambdaTask(
		[this] -> Task
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

			SetupSprites();

			co_return;
		}
	);
}

//------------------------------------------------------------------------------
void ChoiceSystem::Update()
{
	if (m_game->TasksInProgress())
	{
		return;
	}

	// TODO: read controller inputs, and return a choice value on selection
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
		for (u16 sprN = 0; sprN < c_spritesPerChoice; ++sprN)
		{
			auto [id, spr] = m_game->Sprites().AddSprite(
				SpriteSize::r1c4,
				TILE_ATTR_FULL(PAL3, true, false, false, tileIndex + (4 * sprN))
			);

			spr.SetX(c_choiceXPos + (4 * c_pixelsPerTile * sprN));
			spr.SetY(i * 16);
			spr.SetZ(z++);

			m_choiceTextSprites[(i * c_spritesPerChoice) + sprN] = id;
		}

		tileIndex += c_tilesPerChoice;
	}

	// Arrow sprite
	/*{
		auto [id, spr] = m_game->Sprites().AddSprite(
			SpriteSize::r1c1,
			TILE_ATTR_FULL(PAL3, true, false, false, m_game->Sprites().InsertMiscTiles(misc_spr))
		);

		spr.SetVisible(false);
		spr.SetX(304);
		spr.SetY(c_textPosDown + (c_textFramePos + 1 + 2) * 8 + (c_lineSeparation * 2) + 4);

		m_nextArrow = id;
	}*/
}

}