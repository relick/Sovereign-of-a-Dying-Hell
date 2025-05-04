#pragma once

#include "Declare.hpp"
#include "Debug.hpp"

#include <array>
#include <memory>

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

static_assert(sizeof(VRAMSprite) == 8);

//------------------------------------------------------------------------------
struct ExtraSprite
{
    bool m_visible{ true };
    s8 m_z{ 0 };
};

//------------------------------------------------------------------------------
class Sprites
{
    friend SpriteManager;

    // SpriteID is an index to m_sprites/m_vramSprites, which are stable data arrays
    // The first entry (index 0) is reserved by an invisible sprite in order to allow for the stable data with z sorting.
    std::array<bool, c_maxSpriteCount> m_spritesInUse{};
    std::array<VRAMSprite, c_maxSpriteCount> m_vramSpriteData{};
    std::array<ExtraSprite, c_maxSpriteCount> m_extraSpriteData{};

    u16 m_count{ 0 };

    void AddLink(SpriteID::Core i_index)
    {
        if (!m_extraSpriteData[i_index].m_visible) { return; }

        s8 const thisZ = m_extraSpriteData[i_index].m_z;
        u8 lowerIndex = 0;
        s8 lowerZ = INT8_MIN;
        for (u8 i = 1; i < c_maxSpriteCount; ++i)
        {
            if (m_spritesInUse[i] && m_extraSpriteData[i].m_visible)
            {
                s8 const curZ = m_extraSpriteData[i].m_z;
                if (lowerZ <= curZ && curZ < thisZ)
                {
                    lowerIndex = i;
                    lowerZ = curZ;
                }
            }
        }

        Assert(lowerIndex != i_index, "Must not link to self!");

        m_vramSpriteData[i_index].m_link = m_vramSpriteData[lowerIndex].m_link;
        m_vramSpriteData[lowerIndex].m_link = i_index;
    }

    void RemoveLink(SpriteID::Core i_index)
    {
        for (u8 i = 0; i < c_maxSpriteCount; ++i)
        {
            if (m_spritesInUse[i] && m_vramSpriteData[i].m_link == i_index)
            {
                m_vramSpriteData[i].m_link = m_vramSpriteData[i_index].m_link;
                break;
            }
        }

        m_vramSpriteData[i_index].m_link = 0;
    }

public:
    Sprites()
        // Set first entry in each array, as data for reserve sprite that links to the others
        : m_spritesInUse{ { true } }
        , m_vramSpriteData{ { { .m_size = SpriteSize::r1c1, .m_tileAttr = TILE_ATTR_FULL(0, 0, 0, 0, c_reservedClearTile), } } }
        , m_extraSpriteData{ { { .m_visible = true, .m_z = INT8_MIN, } } }
    {
    }

    void ClearAllSprites() { std::ranges::fill(m_spritesInUse, false); m_spritesInUse[0] = true; m_count = 0; }
    bool ValidSprite(SpriteID i_id) const { return i_id.Valid() && i_id.Get() > 0 && m_spritesInUse[i_id.Get()]; }

    VRAMSprite& VRAMSpriteData(SpriteID i_id) { return m_vramSpriteData[i_id.Get()]; }
    ExtraSprite& ExtraSpriteData(SpriteID i_id) { return m_extraSpriteData[i_id.Get()]; }

    SpriteID NewSprite(bool i_visible)
    {
        if (m_count == c_maxSpriteCount - 1)
        {
            Error("Too many sprites"); // TODO: actually handle properly?
            return SpriteID();
        }

        SpriteID::Core id = 1;
        while (id < c_maxSpriteCount && m_spritesInUse[id])
        {
            ++id;
        }

        // Before we set usage to true, reset the data and fill the link
        m_vramSpriteData[id] = {};
        m_extraSpriteData[id] = { .m_visible = i_visible, };

        AddLink(id);
        m_spritesInUse[id] = true;

        ++m_count;

        return SpriteID(id);
    }

    void UpdateVisibility(SpriteID i_id, bool i_visible)
    {
        if (m_extraSpriteData[i_id.Get()].m_visible == i_visible)
        {
            return;
        }

        m_extraSpriteData[i_id.Get()].m_visible = i_visible;
        if (!i_visible)
        {
            // Remove links if becoming non-visible
            RemoveLink(i_id.Get());
        }

        // Insert to proper Z pos if becoming visible, will not do anything if now invisible
        AddLink(i_id.Get());
    }

    void UpdateZ(SpriteID i_id, s8 i_newZ)
    {
        if (m_extraSpriteData[i_id.Get()].m_z == i_newZ)
        {
            return;
        }

        // Easiest way to insert to correct position with the new Z is to remove and add link
        RemoveLink(i_id.Get());
        m_extraSpriteData[i_id.Get()].m_z = i_newZ;
        AddLink(i_id.Get());
    }

    void RemoveSprite(SpriteID i_id)
    {
        Assert(i_id.Get() != 0, "Must not remove sprite 0!");

        // Join up the links either side of this sprite
        // Note there will always be something linking to this sprite, because we should never remove sprite 0
        RemoveLink(i_id.Get());

        m_spritesInUse[i_id.Get()] = false;

#if DEBUG
        for (u8 i = 0; i < c_maxSpriteCount; ++i)
        {
            Assert(!m_spritesInUse[i] || m_vramSpriteData[i].m_link != i_id.Get(), "%u linked to %u, which should be removed!", i, i_id.Get());
        }
#endif

        --m_count;
    }
};

//------------------------------------------------------------------------------
class SpriteManager
{
    friend SpriteHandle;

    // Store in allocated mem as it's quite a bit of memory
    std::unique_ptr<Sprites> m_sprites;

    u16 m_miscSpriteTilesIndex{0};

    bool m_spriteDataEdited{ false };

public:
    SpriteManager()
        : m_sprites{ new Sprites{} }
    {
    }

    void Update(Game& io_game);

    SpriteHandle AddSprite(SpriteSize i_size, u16 i_tileAttr);
    SpriteHandle AddInvisibleSprite(SpriteSize i_size, u16 i_tileAttr);
    [[nodiscard]] SpriteID RemoveSprite(SpriteID i_id); // e.g. id = RemoveSprite(id); to clear the id value
    [[nodiscard]] SpriteHandle RemoveSprite(SpriteHandle i_handle); // e.g. id = RemoveSprite(id); to clear the id value
    void ClearAllSprites();

    SpriteHandle EditSprite(SpriteID i_id);

    u16 InsertMiscTiles(TileSet const& i_tileset);
};

class SpriteHandle
{
    SpriteID m_spriteID;
    VRAMSprite* m_vramSprite{ nullptr };
    SpriteManager* m_spriteManager{ nullptr };

    void Edited() const { m_spriteManager->m_spriteDataEdited = true; }

public:
    SpriteHandle() = default;
    SpriteHandle(SpriteID i_spriteID, VRAMSprite& i_vramSprite, SpriteManager& i_spriteMan) : m_spriteID{ i_spriteID }, m_vramSprite{ &i_vramSprite }, m_spriteManager{ &i_spriteMan } {}

    SpriteID ID() const { return m_spriteID; }

    bool IsVisible() const { return m_spriteManager->m_sprites->ExtraSpriteData(m_spriteID).m_visible; }
    // TODO: add more accessors

    void SetVisible(bool i_visible) { Edited(); m_spriteManager->m_sprites->UpdateVisibility(m_spriteID, i_visible); }
    void SetX(s16 i_x) { Edited(); m_vramSprite->m_xPlus128 = std::min(static_cast<u16>(i_x + 128), u16{ 511 }); }
    void SetY(s16 i_y) { Edited(); m_vramSprite->m_yPlus128 = std::min(static_cast<u16>(i_y + 128), u16{ 511 }); }
    void SetZ(s8 i_z) { Edited(); m_spriteManager->m_sprites->UpdateZ(m_spriteID, i_z); }
    void SetFirstTileIndex(u16 i_index) { Edited(); m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_INDEX_MASK) | (i_index & TILE_INDEX_MASK); }
    void SetSize(SpriteSize i_size) { Edited(); m_vramSprite->m_size = i_size; }
    void SetPriority(bool i_high) { Edited(); m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_PRIORITY_MASK) | (u16{ i_high } << TILE_ATTR_PRIORITY_SFT); }
    void SetFlipH(bool i_flip) { Edited(); m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_HFLIP_MASK) | (u16{ i_flip } << TILE_ATTR_HFLIP_SFT); }
    void SetFlipV(bool i_flip) { Edited(); m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_VFLIP_MASK) | (u16{ i_flip } << TILE_ATTR_VFLIP_SFT); }
    void SetPalette(SpritePalette i_pal) { Edited(); m_vramSprite->m_tileAttr = (m_vramSprite->m_tileAttr & ~TILE_ATTR_PALETTE_MASK) | (static_cast<u16>(i_pal) << TILE_ATTR_PALETTE_SFT); }
};

}