#pragma once

#include "Declare.hpp"

#include <span>
#include <utility>
#include <cstdlib>

namespace Game
{

class AnimFrameDuration
{
	u16 m_minDuration{ 0 };
	u16 m_maxDuration{ 0 };

	constexpr AnimFrameDuration(u16 i_minDuration, u16 i_maxDuration)
		: m_minDuration{ i_minDuration }
		, m_maxDuration{ i_maxDuration }
	{}

public:
	constexpr static AnimFrameDuration Infinite()
	{
		return { 0, 0 };
	}

	constexpr static AnimFrameDuration Fixed(u16 i_duration)
	{
		return { std::max<u16>(i_duration, 1), std::max<u16>(i_duration, 1) };
	}

	constexpr static AnimFrameDuration Variable(u16 i_minDuration, u16 i_maxDuration)
	{
		return { std::max<u16>(i_minDuration, 1), std::max<u16>(i_maxDuration, std::max<u16>(i_minDuration, 1)) };
	}

	constexpr u16 Get() const
	{
		if (m_minDuration == m_maxDuration)
		{
			return m_minDuration;
		}

		// TODO: don't use rand(), and definitely not like this!
		f16 const r = rand() & FIX16_FRAC_MASK;
		f16 const mul = fix16Mul(r, intToFix16(m_maxDuration - m_minDuration));
		return std::max<u16>(fix16ToInt(mul) + m_minDuration, 1);
	}
};

struct AnimFrame
{
	TileMap const* m_tilemap{ nullptr };
	AnimFrameDuration m_duration;
	u16 m_xOffset{ 0 };
	u16 m_yOffset{ 0 };
	u16 m_nextIndex{ UINT16_MAX };
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

inline constexpr Character c_charaLHidden = { "?????", true, {}, };
inline constexpr Character c_charaRHidden = { "?????", false, {}, };

}