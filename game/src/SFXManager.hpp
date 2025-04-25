#pragma once

#include "Declare.hpp"

namespace Game
{

// Mapping from 
enum class Channel : u8
{
    TextBeeps = SOUND_PCM_CH2,
    Menus = SOUND_PCM_CH3,
    OneShots = SOUND_PCM_CH4,
};

class SFXManager
{
    struct SFXData
    {
        bool m_used{false};
        Channel m_channel{};
    };

    // Allow for up to 64 SFX
    std::array<SFXData, 64> m_sfxData;

    // IDs 0-63 reserved for use by music, so we add 64 to the index from sfxData
    static u8 SFXIDToXGMID(SFXID i_id) { return i_id.Get() + 64; }
public:
    SFXID AddSFX(u8 const* i_sample, u32 i_len, Channel i_channel);
    void RemoveSFX(SFXID i_id);

    bool IsPlaying(SFXID i_id) const; // Checks channel for given SFX
    bool IsPlaying(Channel i_channel) const;
    void PlaySFX(SFXID i_id, u8 i_priority = 8) const;
    void StopSFX(SFXID i_id) const; // Stops channel for given SFX
    void StopSFX(Channel i_channel) const;
};

}