#include "SpriteManager.hpp"

#include "Debug.hpp"
#include "Game.hpp"

#include <algorithm>
#include <ranges>

namespace Game
{

inline constexpr u16 c_miscTilesBaseIndex = c_extraTilesStart;
inline constexpr u16 c_maxMiscTiles = c_extraTilesCount;
inline constexpr VRAMSprite c_noSprites{};

//------------------------------------------------------------------------------
void SpriteManager::UpdateMapping
(
)
{
    Assert(m_sprites.size() == m_spriteMapping.SpriteCount(), "Sprite mapping mismatch");
    Assert(m_vramSprites.size() == m_spriteMapping.SpriteCount(), "Sprite mapping mismatch");

    for (u16 i = 0; i < m_sprites.size(); ++i)
    {
        m_spriteMapping[m_sprites[i].m_id] = i;
    }
}

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
        UpdateMapping();
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
            }, TaskPriority::Sprites);
        }
        else
        {
            io_game.QueueFunctionTask([] -> Task {
                
                while (!DMA_queueDmaFast(DMA_VRAM, (void*)&c_noSprites, VDP_getSpriteListAddress(), (sizeof(VRAMSprite) >> 1), 2))
                {
                    co_yield{};
                }
                co_return;
            }(), TaskPriority::Sprites);
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
    SpriteID const newID = m_spriteMapping.NewSprite();
    Assert(newID.Valid(), "Failed to make sprite");

    Sprite& spr = m_sprites.emplace_back();
    VRAMSprite& vSpr = m_vramSprites.emplace_back();

    spr.m_id = newID;
    vSpr.m_size = i_size;
    vSpr.m_tileAttr = i_tileAttr;

    m_spritesAdded = true;
    return { spr.m_id, { spr, vSpr, *this } };
}

//------------------------------------------------------------------------------
SpriteID SpriteManager::RemoveSprite
(
    SpriteID i_id
)
{
    if (!i_id.Valid()) { return SpriteID(); }

    Assert(m_spriteMapping.ValidSprite(i_id), "Tried to remove a sprite that does not exist");
    u16 const index = m_spriteMapping[i_id];

    m_vramSprites.erase(m_vramSprites.begin() + index);
    m_sprites.erase(m_sprites.begin() + index);
    m_spriteMapping.RemoveSprite(i_id);

    m_spritesRemoved = true;
    UpdateMapping();

    return SpriteID();
}

//------------------------------------------------------------------------------
void SpriteManager::ClearAllSprites
(
)
{
    m_vramSprites.clear();
    m_sprites.clear();
    m_spriteMapping.ClearAllSprites();

    m_spritesRemoved = true;
}

//------------------------------------------------------------------------------
EditableSpriteData SpriteManager::EditSpriteData
(
    SpriteID i_id
)
{
    Assert(m_spriteMapping.ValidSprite(i_id), "Tried to edit a sprite that does not exist");
    u16 const index = m_spriteMapping[i_id];

    m_spriteDataEdited = true;
    return { m_sprites[index], m_vramSprites[index], *this };
}

//------------------------------------------------------------------------------
u16 SpriteManager::InsertMiscTiles
(
    TileSet const& i_tileset
)
{
    Assert(i_tileset.numTile <= c_maxMiscTiles, "Too many tiles in misc tiles request to fit into misc region");

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