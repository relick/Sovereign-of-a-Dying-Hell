#include "SpriteManager.hpp"
#include "Debug.hpp"

#include <genesis.h>

#include <algorithm>

namespace Game
{

inline constexpr u16 c_miscTilesBaseAddress = 0xF680; // Sprite list is last thing placed in VRAM, and it ends at F680.
inline constexpr u16 c_miscTilesBaseIndex = c_miscTilesBaseAddress / 32;
inline constexpr u16 c_maxMiscTiles = u16((0x10000 - u32(c_miscTilesBaseAddress)) / 32);

//------------------------------------------------------------------------------
void SpriteManager::Update()
{
    // Sort by Z (lower is drawn first)
    std::sort(m_sprites.begin(), m_sprites.end(), [](Sprite const& sprA, Sprite const& sprB) { return sprA.m_data.m_z < sprB.m_data.m_z; });

    u16 vramI = 0;
    for(auto const& [id, spr] : m_sprites)
    {
        if(vramI == m_vramSprites.size())
        {
            break;
        }

        if(spr.m_visible)
        {
            VRAMSprite& vSpr = m_vramSprites[vramI++];
            vSpr.m_yPlus128 = u16(128 + spr.m_y);
            vSpr.m_size = spr.m_size;
            vSpr.m_link = vramI;
            vSpr.m_tileAttr = TILE_ATTR_FULL(
                u8(spr.m_palette),
                spr.m_highPriority ? 1 : 0,
                spr.m_flipV ? 1 : 0,
                spr.m_flipH ? 1 : 0,
                spr.m_firstTileIndex
                );
            vSpr.m_xPlus128 = u16(128 + spr.m_x);
        }
    }

    if(vramI > 0)
    {
        m_vramSprites[vramI - 1].m_link = 0;
    }
    else
    {
        m_vramSprites[0] = VRAMSprite{};
        vramI = 1;
    }

	DMA_queueDmaFast(DMA_VRAM, &m_vramSprites, VDP_getSpriteListAddress(), vramI * (sizeof(VRAMSprite) >> 1), 2);
}

//------------------------------------------------------------------------------
std::pair<SpriteID, SpriteData&> SpriteManager::AddSprite
(
    SpriteSize i_size,
    u16 i_firstTileIndex
)
{
    Sprite& spr = m_sprites.emplace_back();
    spr.m_id = m_nextSpriteID++;
    spr.m_data.m_size = i_size;
    spr.m_data.m_firstTileIndex = i_firstTileIndex;
    return { spr.m_id, spr.m_data };
}

//------------------------------------------------------------------------------
std::pair<SpriteID, SpriteData&> SpriteManager::AddSprite
(
    SpriteData&& i_initData
)
{
    Sprite& spr = m_sprites.emplace_back(Sprite{m_nextSpriteID++, std::move(i_initData)});
    return { spr.m_id, spr.m_data };
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
    }
}

//------------------------------------------------------------------------------
SpriteData& SpriteManager::EditSpriteData
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
        return sprI->m_data;
    }

    Error("Tried to edit a sprite that no longer exists");
    return m_sprites.back().m_data;
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