#pragma once

#include "Declare.hpp"
#include "Debug.hpp"

#include <algorithm>
#include <vector>

namespace Game
{

//------------------------------------------------------------------------------
struct VRAMSprite
{
	u16 m_yPlus128{0};
	SpriteSize m_size{0};
	u8 m_link{0};
	u16 m_tileAttr{0};
	u16 m_xPlus128{0};
};

//------------------------------------------------------------------------------
struct Sprite
{
    SpriteID m_id;

    bool m_visible{ true };
    s8 m_z{ 0 };
};

//------------------------------------------------------------------------------
class SpriteMapping
{
    std::vector<u16> m_indices;
    u16 m_count{ 0 };

public:
    SpriteMapping() { m_indices.resize(c_maxSpriteCount, UINT16_MAX); }
    void ClearAllSprites() { std::ranges::fill(m_indices, UINT16_MAX); }
    u16 SpriteCount() const { return m_count; }
    bool ValidSprite(SpriteID i_id) const { return i_id.Valid() && m_indices[i_id.Get()] < UINT16_MAX; }
    u16& operator[](SpriteID i_id) { return m_indices[i_id.Get()]; }

    SpriteID NewSprite()
    {
        SpriteID::Core id = 0;
        while (id < c_maxSpriteCount && m_indices[id] < UINT16_MAX)
        {
            ++id;
        }
        if (id == c_maxSpriteCount)
        { 
            Error("Too many sprites"); // TODO: actually handle properly?
            return SpriteID();
        }

        m_indices[id] = SpriteCount();
        ++m_count;

        return SpriteID(id);
    }

    void RemoveSprite(SpriteID i_id)
    {
        m_indices[i_id.Get()] = UINT16_MAX;
        --m_count;
    }
};

class EditableSpriteData;

//------------------------------------------------------------------------------
class SpriteManager
{
    friend EditableSpriteData;

    // SpriteID -> Sprite/VRAMSprite indices
    SpriteMapping m_spriteMapping;
    std::vector<Sprite> m_sprites;
    std::vector<VRAMSprite> m_vramSprites;

    u16 m_miscSpriteTilesIndex{0};

    bool m_spritesAdded{ false };
    bool m_spritesRemoved{ false };
    bool m_spriteDataEdited{ false };
    bool m_visibilitiesChanged{ false };
    bool m_orderingChanged{ false };

    // These may not match with the full size of the vectors, depending on if the first/last sections of sprites are visible or not
    u16 m_firstSpriteIndex{ UINT16_MAX };
    u16 m_lastSpriteIndex{ 0 };

    void UpdateMapping();

public:
    SpriteManager()
    {
        m_sprites.reserve(c_maxSpriteCount);
        m_vramSprites.reserve(c_maxSpriteCount);
    }

    void Update(Game& io_game);

    std::pair<SpriteID, EditableSpriteData> AddSprite(SpriteSize i_size, u16 i_tileAttr);
    [[nodiscard]] SpriteID RemoveSprite(SpriteID i_id); // e.g. id = RemoveSprite(id); to clear the id value
    void ClearAllSprites();

    EditableSpriteData EditSpriteData(SpriteID i_id);

    u16 InsertMiscTiles(TileSet const& i_tileset);
};

class EditableSpriteData
{
    Sprite* m_sprite;
    VRAMSprite* m_vramSprite;
    SpriteManager* m_spriteManager;
public:
    EditableSpriteData(Sprite& i_sprite, VRAMSprite& i_vramSprite, SpriteManager& i_spriteMan) : m_sprite{ &i_sprite }, m_vramSprite{ &i_vramSprite }, m_spriteManager{ &i_spriteMan } {}

    bool IsVisible() const { return m_sprite->m_visible; }
    // TODO: add more accessors

    void SetVisible(bool i_visible) { m_spriteManager->m_visibilitiesChanged = (m_sprite->m_visible != i_visible); m_sprite->m_visible = i_visible; }
    void SetX(s16 i_x) { m_vramSprite->m_xPlus128 = std::min(static_cast<u16>(i_x + 128), u16{ 511 }); }
    void SetY(s16 i_y) { m_vramSprite->m_yPlus128 = std::min(static_cast<u16>(i_y + 128), u16{ 511 }); }
    void SetZ(s8 m_z) { m_spriteManager->m_orderingChanged = (m_sprite->m_z != m_z); m_sprite->m_z = m_z; }
    void SetFirstTileIndex(u16 i_index) { m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_INDEX_MASK) | (i_index & TILE_INDEX_MASK); }
    void SetSize(SpriteSize i_size) { m_vramSprite->m_size = i_size; }
    void SetPriority(bool i_high) { m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_PRIORITY_MASK) | (u16{ i_high } << TILE_ATTR_PRIORITY_SFT); }
    void SetFlipH(bool i_flip) { m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_HFLIP_MASK) | (u16{ i_flip } << TILE_ATTR_HFLIP_SFT); }
    void SetFlipV(bool i_flip) { m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_VFLIP_MASK) | (u16{ i_flip } << TILE_ATTR_VFLIP_SFT); }
    void SetPalette(SpritePalette i_pal) { m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_PALETTE_MASK) | (static_cast<u16>(i_pal) << TILE_ATTR_PALETTE_SFT); }
};

}