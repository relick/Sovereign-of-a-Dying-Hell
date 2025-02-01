#include "CharacterData.hpp"

#include <algorithm>
#include <cstring>

namespace Game
{

//------------------------------------------------------------------------------
CharacterID CharacterData::AddCharacter
(
	Character const* i_character
)
{
	m_characters.push_back(i_character);
	return static_cast<u8>(m_characters.size() - 1);
}

//------------------------------------------------------------------------------
void CharacterData::Clear()
{
	m_characters.clear();
}

//------------------------------------------------------------------------------
Character const* CharacterData::GetCharacter
(
	CharacterID i_charID
)
{
	return i_charID < m_characters.size()
		? m_characters[i_charID]
		: nullptr;
}

//------------------------------------------------------------------------------
std::pair<Character const*, Pose const*> CharacterData::GetPose
(
	CharacterID i_charID,
	PoseID i_poseID
)
{
	return i_charID < m_characters.size() && i_poseID < m_characters[i_charID]->m_poses.size()
		? std::pair<Character const*, Pose const*>{ m_characters[i_charID], &m_characters[i_charID]->m_poses[i_poseID] }
	: std::pair<Character const*, Pose const*>{ nullptr, nullptr };
}

}