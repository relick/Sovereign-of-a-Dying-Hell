#include "SpriteManager.hpp"
#include "Debug.hpp"
#include "Game.hpp"

#include <genesis.h>

#include <algorithm>
#include <ranges>

namespace Game
{

inline constexpr u16 c_miscTilesBaseAddress = 0xF680; // Sprite list is last thing placed in VRAM, and it ends at F680.
inline constexpr u16 c_miscTilesBaseIndex = c_miscTilesBaseAddress / 32;
inline constexpr u16 c_maxMiscTiles = u16((0x10000 - u32(c_miscTilesBaseAddress)) / 32);
inline constexpr VRAMSprite c_noSprites{};

//------------------------------------------------------------------------------
void SpriteManager::Update
(
    Game& io_game
)
{
    bool const needsSorting = m_spritesAdded || m_orderingChanged;
    bool const needsLinksUpdating = needsSorting || m_spritesRemoved || m_visibilitiesChanged;
    bool const needsDMA = m_spriteDataEdited || needsLinksUpdating;

    // Sort by Z if needed (lower is drawn first)
    if (needsSorting)
    {
        auto zipped = std::views::zip(m_sprites, m_vramSprites);
        std::ranges::sort(zipped, [](auto const& sprA, auto const& sprB) { return std::get<0>(sprA).m_z < std::get<0>(sprB).m_z; });
    }

    // Update links if needed
    if (needsLinksUpdating)
    {
        m_firstSpriteIndex = UINT16_MAX;
        m_lastSpriteIndex = 0;
        for (u16 i = 0; i < m_sprites.size(); ++i)
        {
            if (m_sprites[i].m_visible)
            {
                m_firstSpriteIndex = i;
                break;
            }
        }

        for (u16 i = m_sprites.size(); i > 0; --i)
        {
            if (m_sprites[i - 1].m_visible)
            {
                m_lastSpriteIndex = i - 1;
                break;
            }
        }

        if (m_firstSpriteIndex <= m_lastSpriteIndex)
        {
            u16 prevVisibleIndex = m_firstSpriteIndex;
            for (u16 i = m_firstSpriteIndex + 1; i <= m_lastSpriteIndex; ++i)
            {
                if (m_sprites[i].m_visible)
                {
                    m_vramSprites[prevVisibleIndex].m_link = i - m_firstSpriteIndex;
                    prevVisibleIndex = i;
                }
            }

            m_vramSprites[m_lastSpriteIndex].m_link = 0;
        }
    }

    // Queue DMA if needed
    if (needsDMA)
    {
        if (m_firstSpriteIndex <= m_lastSpriteIndex)
        {
            io_game.QueueLambdaTask([this] -> Task {
                while (!DMA_queueDmaFast(DMA_VRAM, m_vramSprites.data() + m_firstSpriteIndex, VDP_getSpriteListAddress(), (m_lastSpriteIndex + 1 - m_firstSpriteIndex) * (sizeof(VRAMSprite) >> 1), 2))
                {
                    co_yield{};
                }
                co_return;
            });
        }
        else
        {
            io_game.QueueFunctionTask([] -> Task {
                
                while (!DMA_copyAndQueueDma(DMA_VRAM, (void*)&c_noSprites, VDP_getSpriteListAddress(), (sizeof(VRAMSprite) >> 1), 2))
                {
                    co_yield{};
                }
                co_return;
            }());
        }
    }

    m_spritesAdded = false;
    m_spritesRemoved = false;
    m_spriteDataEdited = false;
    m_visibilitiesChanged = false;
    m_orderingChanged = false;
}

//------------------------------------------------------------------------------
std::pair<SpriteID, EditableSpriteData> SpriteManager::AddSprite
(
    SpriteSize i_size,
    u16 i_tileAttr
)
{
    Sprite& spr = m_sprites.emplace_back();
    VRAMSprite& vSpr = m_vramSprites.emplace_back();
    spr.m_id = m_nextSpriteID++;
    vSpr.m_size = i_size;
    vSpr.m_tileAttr = i_tileAttr;

    m_spritesAdded = true;

    return { spr.m_id, { spr, vSpr, *this } };
}

//------------------------------------------------------------------------------
std::pair<SpriteID, EditableSpriteData> SpriteManager::CloneSprite
(
    SpriteID i_id
)
{
    auto clonedSprI = std::find_if(
        m_sprites.begin(),
        m_sprites.end(),
        [i_id](auto const& spr) { return spr.m_id == i_id; }
    );

    if (clonedSprI != m_sprites.end())
    {
        Sprite& newSpr = m_sprites.emplace_back(*clonedSprI);
        VRAMSprite& vSpr = m_vramSprites.emplace_back(*(m_vramSprites.begin() + std::distance(m_sprites.begin(), clonedSprI)));
        m_spritesAdded = true;

        return { newSpr.m_id, { newSpr, vSpr, * this } };
    }

    return { {}, { m_sprites.back(), m_vramSprites.back(), *this } };
}

//------------------------------------------------------------------------------
void SpriteManager::RemoveSprite
(
    SpriteID i_id
)
{
	auto sprI = std::find_if(
		m_sprites.begin(),
		m_sprites.end(),
		[i_id](auto const& spr) { return spr.m_id == i_id; }
	);

    if(sprI != m_sprites.end())
    {
        m_sprites.erase(sprI);
        m_spritesRemoved = true;
    }
}

//------------------------------------------------------------------------------
void SpriteManager::ClearAllSprites
(
)
{
    m_sprites.clear();
    m_vramSprites.clear();
    m_spritesRemoved = true;
}

//------------------------------------------------------------------------------
EditableSpriteData SpriteManager::EditSpriteData
(
    SpriteID i_id
)
{
	auto sprI = std::find_if(
		m_sprites.begin(),
		m_sprites.end(),
		[i_id](auto const& spr) { return spr.m_id == i_id; }
	);

    if(sprI != m_sprites.end())
    {
        m_spriteDataEdited = true;
        return { *sprI, *(m_vramSprites.begin() + std::distance(m_sprites.begin(), sprI)), * this };
    }

    Error("Tried to edit a sprite that no longer exists");
    return { m_sprites.back(), m_vramSprites.back(), *this };
}

//------------------------------------------------------------------------------
u16 SpriteManager::InsertMiscTiles
(
    TileSet const& i_tileset
)
{
    if(m_miscSpriteTilesIndex + i_tileset.numTile > c_maxMiscTiles)
    {
        // start overwriting from the start!
        m_miscSpriteTilesIndex = 0;
    }

    u16 const tileIndex = m_miscSpriteTilesIndex + c_miscTilesBaseIndex;
    m_miscSpriteTilesIndex += i_tileset.numTile;

    VDP_loadTileSet(&i_tileset, tileIndex & TILE_INDEX_MASK, DMA_QUEUE);

    return tileIndex;
}

}