#pragma once

#include "Declare.hpp"

#include <genesis.h>

#include <vector>

namespace Game
{

struct Pose
{
	char const* m_name{ nullptr };
	Image const* m_image{ nullptr };
	Palette const* m_namePal{ nullptr };
	Palette const* m_textPal{ nullptr };
};

struct Character
{
	char const* m_name{ nullptr };
	char const* m_displayName{ nullptr };
	bool m_showOnLeft{ false };
	std::vector<Pose> m_poses;
};

// Simple data manager for scripts to dump into and the VN world to process
class CharacterData
{
	std::vector<Character> m_characters;

public:
	void AddCharacter(char const* i_name, char const* i_displayName, bool i_showOnLeft);
	void AddPose(char const* i_charName, char const* i_poseName, Image const* i_image, Palette const* i_namePal, Palette const* i_textPal);

	void Clear();

	Character const* FindCharacter(char const* i_charName);
	std::pair<Character const*, Pose const*> FindPose(char const* i_charName, char const* i_poseName);
};

}