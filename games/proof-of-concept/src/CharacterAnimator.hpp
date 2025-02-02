#pragma once

#include "Declare.hpp"
#include "CharacterData.hpp"

#include <optional>

namespace Game
{

class CharacterAnimator
{
	struct AnimContext
	{
		Pose const* m_pose{ nullptr };
		u16 m_baseTile{ 0 };
		u16 m_currentFrameIndex{ 0 };
		u16 m_ticks{ 0 };

		AnimContext(Pose const* i_pose, u16 i_baseTile) : m_pose{ i_pose }, m_baseTile{ i_baseTile } {}
	};

	std::optional<AnimContext> m_context;

public:
	void Update(Game& io_game);

	void StartAnimation(Pose const* i_pose, u16 i_baseTile) { m_context = AnimContext{ i_pose, i_baseTile }; }
	void StopAnimation() { m_context = std::nullopt; }
};

}