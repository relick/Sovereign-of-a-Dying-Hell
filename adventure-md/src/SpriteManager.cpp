#include "adventure-md/SpriteManager.hpp"

#include "adventure-md/Debug.hpp"
#include "adventure-md/Game.hpp"

namespace Game
{

inline constexpr u16 c_miscTilesBaseIndex = c_extraTilesStart;
inline constexpr u16 c_maxMiscTiles = c_extraTilesCount;

//------------------------------------------------------------------------------
void SpriteManager::Update
(
    Game& io_game
)
{
    // Queue DMA if needed
    if (m_spriteDataEdited)
    {
        if (DMA_queueDmaFast(DMA_VRAM, m_sprites->m_vramSpriteData.data(), VDP_getSpriteListAddress(), m_sprites->m_vramSpriteData.size() * (sizeof(VRAMSprite) >> 1), 2))
        {
            m_spriteDataEdited = false;
        }
    }
}

//------------------------------------------------------------------------------
SpriteHandle SpriteManager::AddSprite
(
    SpriteSize i_size,
    u16 i_tileAttr
)
{
    SpriteID const newID = m_sprites->NewSprite(true);
    Assert(newID.Valid(), "Failed to make sprite");

    VRAMSprite& vSpr = m_sprites->VRAMSpriteData(newID);
    vSpr.m_size = i_size;
    vSpr.m_tileAttr = i_tileAttr;

    m_spriteDataEdited = true;

    return { newID, vSpr, *this };
}

//------------------------------------------------------------------------------
SpriteHandle SpriteManager::AddInvisibleSprite
(
    SpriteSize i_size,
    u16 i_tileAttr
)
{
    SpriteID const newID = m_sprites->NewSprite(false);
    Assert(newID.Valid(), "Failed to make sprite");

    VRAMSprite& vSpr = m_sprites->VRAMSpriteData(newID);
    vSpr.m_size = i_size;
    vSpr.m_tileAttr = i_tileAttr;

    m_spriteDataEdited = true;

    return { newID, vSpr, *this };
}

//------------------------------------------------------------------------------
SpriteID SpriteManager::RemoveSprite
(
    SpriteID i_id
)
{
    if (!i_id.Valid()) { return SpriteID(); }

    Assert(m_sprites->ValidSprite(i_id), "Tried to remove a sprite that does not exist");

    m_sprites->RemoveSprite(i_id);
    m_spriteDataEdited = true;

    return SpriteID();
}

//------------------------------------------------------------------------------
SpriteHandle SpriteManager::RemoveSprite
(
    SpriteHandle i_handle
)
{
    if (!i_handle.ID().Valid()) { return SpriteHandle(); }

    Assert(m_sprites->ValidSprite(i_handle.ID()), "Tried to remove a sprite that does not exist");

    m_sprites->RemoveSprite(i_handle.ID());
    m_spriteDataEdited = true;

    return SpriteHandle();
}

//------------------------------------------------------------------------------
void SpriteManager::ClearAllSprites
(
)
{
    m_sprites->ClearAllSprites();
    m_spriteDataEdited = true;
}

//------------------------------------------------------------------------------
SpriteHandle SpriteManager::EditSprite
(
    SpriteID i_id
)
{
    Assert(m_sprites->ValidSprite(i_id), "Tried to edit a sprite that does not exist");
    return { i_id, m_sprites->VRAMSpriteData(i_id), *this };
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