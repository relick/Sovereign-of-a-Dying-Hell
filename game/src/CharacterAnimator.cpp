#include "CharacterAnimator.hpp"

#include "Game.hpp"
#include "TileOps.hpp"

namespace Game
{

CharacterAnimator::AnimContext::AnimContext
(
	Pose const* i_pose,
	u16 i_baseTile
)
	: m_pose{ i_pose }
	, m_baseTile{ i_baseTile }
	, m_currentFrameTicks{ i_pose->m_animation[0].m_duration.Get() }
{}

void CharacterAnimator::Update
(
	Game& io_game
)
{
	if (m_context)
	{
		if (m_context->m_currentFrameTicks == 0)
		{
			// Freeze on this frame
			return;
		}


		++m_context->m_ticks;
		if (m_context->m_ticks >= m_context->m_currentFrameTicks)
		{
			++m_context->m_currentFrameIndex;
			if (m_context->m_currentFrameIndex >= m_context->m_pose->m_animation.size())
			{
				m_context->m_currentFrameIndex = 0;
			}

			AnimFrame const& animFrame = m_context->m_pose->m_animation[m_context->m_currentFrameIndex];

			m_context->m_ticks = 0;
			m_context->m_currentFrameTicks = animFrame.m_duration.Get();

			io_game.QueueFunctionTask(Tiles::SetMap_SubFull(
				VDP_BG_A,
				animFrame.m_tilemap->tilemap,
				animFrame.m_tilemap->w,
				animFrame.m_tilemap->h,
				m_context->m_baseTile,
				animFrame.m_xOffset,
				animFrame.m_yOffset
			), TaskPriority::Animations);
		}
	}
}

}