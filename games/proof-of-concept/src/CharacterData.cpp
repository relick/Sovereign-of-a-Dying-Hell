#include "CharacterData.hpp"

#include <algorithm>
#include <cstring>

namespace Game
{

//------------------------------------------------------------------------------
void CharacterData::AddCharacter
(
	char const* i_name,
	char const* i_displayName,
	bool i_showOnLeft
)
{
	m_characters.push_back({ i_name, i_displayName, i_showOnLeft, {} });
}

//------------------------------------------------------------------------------
void CharacterData::AddPose
(
	char const* i_charName,
	char const* i_poseName,
	Image const* i_image,
	Palette const* i_namePal,
	Palette const* i_textPal
)
{
	auto charI = std::find_if(
		m_characters.begin(),
		m_characters.end(),
		[i_charName](auto const& chara){ return chara.m_name == i_charName || std::strcmp(chara.m_name, i_charName) == 0; }
	);

	if (charI != m_characters.end())
	{
		charI->m_poses.push_back({ i_poseName, i_image, i_namePal, i_textPal });
	}
}

//------------------------------------------------------------------------------
void CharacterData::Clear()
{
	m_characters.clear();
}

//------------------------------------------------------------------------------
Character const* CharacterData::FindCharacter
(
	char const* i_charName
)
{
	if(i_charName)
	{
		auto charI = std::find_if(
			m_characters.begin(),
			m_characters.end(),
			[i_charName](auto const& chara) { return chara.m_name == i_charName || std::strcmp(chara.m_name, i_charName) == 0; }
		);

		if (charI != m_characters.end())
		{
			return &*charI;
		}
	}

	return {};
}

//------------------------------------------------------------------------------
std::pair<Character const*, Pose const*> CharacterData::FindPose
(
	char const* i_charName,
	char const* i_poseName
)
{
	Character const* chara = FindCharacter(i_charName);

	if (chara)
	{
		auto poseI = std::find_if(
			chara->m_poses.begin(),
			chara->m_poses.end(),
			[i_poseName](auto const& pose) { return pose.m_name == i_poseName || std::strcmp(pose.m_name, i_poseName) == 0; }
		);

		if (poseI != chara->m_poses.end())
		{
			return { chara, &*poseI };
		}
	}

	return {};
}

}