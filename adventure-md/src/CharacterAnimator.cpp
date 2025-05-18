#include "adventure-md/CharacterAnimator.hpp"

#include "adventure-md/Game.hpp"
#include "adventure-md/TileOps.hpp"

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
			// Step to next frame, using the index provided or just incrementing
			AnimFrame const& prevFrame = m_context->m_pose->m_animation[m_context->m_currentFrameIndex];
			if (prevFrame.m_nextIndex < m_context->m_pose->m_animation.size())
			{
				m_context->m_currentFrameIndex = prevFrame.m_nextIndex;
			}
			else
			{
				++m_context->m_currentFrameIndex;
				if (m_context->m_currentFrameIndex >= m_context->m_pose->m_animation.size())
				{
					m_context->m_currentFrameIndex = 0;
				}
			}

			AnimFrame const& curFrame = m_context->m_pose->m_animation[m_context->m_currentFrameIndex];

			m_context->m_ticks = 0;
			m_context->m_currentFrameTicks = curFrame.m_duration.Get();

			io_game.QueueFunctionTask(Tiles::SetMap_SubFull(
				VDP_BG_A,
				curFrame.m_tilemap->tilemap,
				curFrame.m_tilemap->w,
				curFrame.m_tilemap->h,
				m_context->m_baseTile,
				curFrame.m_xOffset,
				curFrame.m_yOffset
			), TaskPriority::Animations);
		}
	}
}

}