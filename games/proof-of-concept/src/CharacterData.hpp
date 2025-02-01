#pragma once

#include "Declare.hpp"

#include <genesis.h>

#include <array>
#include <span>
#include <vector>

namespace Game
{

struct AnimFrame
{
	TileMap const* m_tilemap{ nullptr };
	u16 m_frameDuration{ 0 }; // 0 => stay on this frame forever
};

template<size_t t_FrameCount>
using Animation = std::array<AnimFrame, t_FrameCount>;

struct Pose
{
	TileSet const* m_tileset{ nullptr };
	Palette const* m_palette{ nullptr };
	std::span<AnimFrame const> m_animation;
};

template<size_t t_PoseCount>
using Poses = std::array<Pose, t_PoseCount>;

struct Character
{
	char const* m_displayName{ nullptr };
	bool m_showOnLeft{ false };
	std::span<Pose const> m_poses;
};

// Simple data manager for scripts to dump into and the VN world to process
class CharacterData
{
	std::vector<Character const*> m_characters;

public:
	CharacterID AddCharacter(Character const* i_character);

	void Clear();

	Character const* GetCharacter(CharacterID i_charID);
	std::pair<Character const*, Pose const*> GetPose(CharacterID i_charID, PoseID i_poseID);
};

}