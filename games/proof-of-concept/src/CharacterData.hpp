#pragma once

#include "Declare.hpp"

#include <genesis.h>

#include <span>

namespace Game
{

struct AnimFrame
{
	TileMap const* m_tilemap{ nullptr };
	u16 m_frameDuration{ 0 }; // 0 => stay on this frame forever
};

struct Pose
{
	TileSet const* m_tileset{ nullptr };
	Palette const* m_palette{ nullptr };
	std::span<AnimFrame const> m_animation;
};

struct Character
{
	char const* m_displayName{ nullptr };
	bool m_showOnLeft{ false };
	std::span<Pose const> m_poses;
};

}