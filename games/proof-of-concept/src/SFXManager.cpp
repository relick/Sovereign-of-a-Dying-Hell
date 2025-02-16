#include "SFXManager.hpp"

#include "Debug.hpp"

#include <algorithm>

namespace Game
{

//------------------------------------------------------------------------------
SFXID SFXManager::AddSFX
(
    u8 const* i_sample,
    u32 i_len,
    Channel i_channel
)
{
    auto nextFreeI = std::find_if(m_sfxData.begin(), m_sfxData.end(), [](auto const& o) { return !o.m_used; });
    if(nextFreeI == m_sfxData.end())
    {
        Error("Ran out of SFX slots!");
        return -1;
    }
    SFXID const id = std::distance(m_sfxData.begin(), nextFreeI);
    XGM_setPCM(SFXIDToXGMID(id), i_sample, i_len);

    nextFreeI->m_used = true;
    nextFreeI->m_channel = i_channel;

    return id;
}

//------------------------------------------------------------------------------
void SFXManager::RemoveSFX
(
    SFXID i_id
)
{
    m_sfxData[i_id].m_used = false;
}

//------------------------------------------------------------------------------
bool SFXManager::IsPlaying
(
    SFXID i_id
) const
{
    return IsPlaying(m_sfxData[i_id].m_channel);
}

//------------------------------------------------------------------------------
bool SFXManager::IsPlaying
(
    Channel i_channel
) const
{
    return XGM_isPlayingPCM(1 << static_cast<u16>(i_channel));
}

//------------------------------------------------------------------------------
void SFXManager::PlaySFX
(
    SFXID i_id,
    u8 i_priority
) const
{
    XGM_startPlayPCM(SFXIDToXGMID(i_id), i_priority, static_cast<u16>(m_sfxData[i_id].m_channel));
}

//------------------------------------------------------------------------------
void SFXManager::StopSFX
(
    SFXID i_id
) const
{
    StopSFX(m_sfxData[i_id].m_channel);
}

//------------------------------------------------------------------------------
void SFXManager::StopSFX
(
    Channel i_channel
) const
{
    XGM_stopPlayPCM(static_cast<u16>(i_channel));
}

}