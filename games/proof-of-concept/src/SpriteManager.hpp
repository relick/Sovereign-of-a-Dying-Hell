#pragma once

#include "Declare.hpp"

#include <array>
#include <vector>

namespace Game
{

//------------------------------------------------------------------------------
struct SpriteData
{
    s16 m_x{0};
    s16 m_y{0};
    u16 m_firstTileIndex{0};
    SpriteSize m_size{SpriteSize::r1c1};
    bool m_visible{true};
    s8 m_z{0};
    bool m_highPriority{false};
    bool m_flipH{false};
    bool m_flipV{false};
    SpritePalette m_palette{SpritePalette::Pal1};
};

struct Sprite
{
    SpriteID m_id{UINT16_MAX};
    SpriteData m_data;
};

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
class SpriteManager
{
    std::vector<Sprite> m_sprites;
    std::array<VRAMSprite, 80> m_vramSprites;
    
    SpriteID m_nextSpriteID{0};

public:
    void Update();

    std::pair<SpriteID, SpriteData&> AddSprite(SpriteSize i_size, u16 i_firstTileIndex);
    std::pair<SpriteID, SpriteData&> AddSprite(SpriteData&& i_initData);
    void RemoveSprite(SpriteID i_id);

    SpriteData& EditSpriteData(SpriteID i_id);
};

}