#pragma once

#include "Declare.hpp"

#include <genesis.h>

#include <vector>

namespace Game
{

struct Pose
{
	Image const* m_image{ nullptr };
};

struct Character
{
	char const* m_displayName{ nullptr };
	bool m_showOnLeft{ false };
	std::vector<Pose> m_poses;
};

// Simple data manager for scripts to dump into and the VN world to process
class CharacterData
{
	std::vector<Character> m_characters;

public:
	CharacterID AddCharacter(char const* i_displayName, bool i_showOnLeft);
	PoseID AddPose(CharacterID i_charID, Image const* i_image);

	void Clear();

	Character const* GetCharacter(CharacterID i_charID);
	std::pair<Character const*, Pose const*> GetPose(CharacterID i_charID, PoseID i_poseID);
};

}