#include "Game.hpp"

#include "Worlds.hpp"

#include <array>

namespace Game
{

#define LOG_WHOLE_FRAME_TIMES (1 && PROFILER)

#if PROFILER
static u32 volatile s_frameLineCount{};
#endif

#if LOG_WHOLE_FRAME_TIMES
static std::array<u32, 6> s_frameLogPoints{};
#endif

//------------------------------------------------------------------------------
Game::Game()
{
	s_vBlankCallbacks.reserve(8);

#if PROFILER
	SYS_setVIntCallback(Game::VIntCallback);
#endif
	SYS_setVBlankCallback(Game::VBlankCallback);

	VDP_setScreenWidth320();
	VDP_setScreenHeight224();

	PreWorldInit();
}

//------------------------------------------------------------------------------
void Game::RequestNextWorld(std::unique_ptr<World> &&i_nextWorld)
{
	m_nextWorld = std::move(i_nextWorld);
}

//------------------------------------------------------------------------------
void Game::Run()
{
	while (true)
	{
		if (!m_nextWorld)
		{
#if LOG_WHOLE_FRAME_TIMES
			s_frameLogPoints[0] = GetVCount();
#endif
			m_curWorld->Run(*this);
			PostWorldFrame();
		}
		else
		{
			if (m_curWorld)
			{
				m_currentWorldRoutine = m_curWorld->Shutdown(*this);
				while (m_currentWorldRoutine)
				{
#if LOG_WHOLE_FRAME_TIMES
					s_frameLogPoints[0] = GetVCount();
#endif
					m_currentWorldRoutine();
					PostWorldFrame();
				}
			}

			std::swap(m_curWorld, m_nextWorld);
			m_nextWorld = nullptr;

			PreWorldInit();

			m_currentWorldRoutine = m_curWorld->Init(*this);
			while (m_currentWorldRoutine)
			{
#if LOG_WHOLE_FRAME_TIMES
				s_frameLogPoints[0] = GetVCount();
#endif
				m_currentWorldRoutine();
				PostWorldFrame();
			}

			m_currentWorldRoutine = {};
		}
	}
}

#if PROFILER
//------------------------------------------------------------------------------
u32 Game::GetVCount()
{
	u8 const vCount = GET_VCOUNTER;
	u8 constexpr offset = 224;
	return u32{ static_cast<u8>(vCount - offset) } + s_frameLineCount;
}
#endif

//------------------------------------------------------------------------------
VBlankCallbackID Game::AddVBlankCallback(std::function<void()>&& i_callback)
{
	s_vBlankCallbacks.push_back({ s_callbackID, std::move(i_callback), });
	return s_callbackID++;
}

//------------------------------------------------------------------------------
void Game::RemoveVBlankCallback(VBlankCallbackID i_callbackID)
{
	auto cbI = std::find_if(
		s_vBlankCallbacks.begin(),
		s_vBlankCallbacks.end(),
		[i_callbackID](auto const& cb) { return cb.first == i_callbackID; }
	);
	
	if(cbI != s_vBlankCallbacks.end())
	{
		s_vBlankCallbacks.erase(cbI);
	}
}

//------------------------------------------------------------------------------
void Game::QueueFunctionTask
(
	Task&& i_task
)
{
	QueueFunctionTask(std::move(i_task), TaskPriority::Normal);
}

//------------------------------------------------------------------------------
void Game::QueueFunctionTask
(
	Task&& i_task,
	TaskPriority i_priority
)
{
	m_tasks.push_back({ std::move(i_task), {}, i_priority, });
	std::push_heap(m_tasks.begin(), m_tasks.end());
}

//------------------------------------------------------------------------------
bool Game::TasksInProgress() const
{
	return DMA_getQueueSize() > 0 || !m_tasks.empty();
}

//------------------------------------------------------------------------------
void Game::VIntCallback()
{
#if PROFILER
	s_frameLineCount += 261;
#endif
#if LOG_WHOLE_FRAME_TIMES
	s_frameLogPoints[4] = GetVCount();
#endif
}

//------------------------------------------------------------------------------
void Game::VBlankCallback()
{
	for (auto const& [id, f] : s_vBlankCallbacks)
	{
		f();
	}
}

//------------------------------------------------------------------------------
void Game::PostWorldFrame()
{
#if LOG_WHOLE_FRAME_TIMES
	s_frameLogPoints[1] = GetVCount();
#endif

	m_sprites.Update(*this);

#if LOG_WHOLE_FRAME_TIMES
	s_frameLogPoints[2] = GetVCount();
#endif

	while (!m_tasks.empty() && !m_tasks.front().m_routine)
	{
		std::pop_heap(m_tasks.begin(), m_tasks.end());
		m_tasks.pop_back();
	}

	if (!m_tasks.empty())
	{
		m_tasks.front().m_routine();
	}

#if LOG_WHOLE_FRAME_TIMES
	s_frameLogPoints[3] = GetVCount();
#endif

	SYS_doVBlankProcess();

#if LOG_WHOLE_FRAME_TIMES
	s_frameLogPoints[5] = GetVCount();

	//kprintf("VCounts: %lx, %lx, %lx, %lx.", s_frameLogPoints[0], s_frameLogPoints[1], s_frameLogPoints[2], s_frameLogPoints[3]);
	kprintf("Game Frame: %u. Sprites: %u. DMA Routines: %u. VBlankProcess (total/blank): (%u, %u).",
		static_cast<u16>(s_frameLogPoints[1] - s_frameLogPoints[0] + (s_frameLogPoints[0] <= s_frameLogPoints[1] ? 0 : 5)),
		static_cast<u16>(s_frameLogPoints[2] - s_frameLogPoints[1] + (s_frameLogPoints[1] <= s_frameLogPoints[2] ? 0 : 5)),
		static_cast<u16>(s_frameLogPoints[3] - s_frameLogPoints[2] + (s_frameLogPoints[2] <= s_frameLogPoints[3] ? 0 : 5)),
		static_cast<u16>(s_frameLogPoints[5] - s_frameLogPoints[3] + (s_frameLogPoints[3] <= s_frameLogPoints[5] ? 0 : 5)),
		static_cast<u16>(s_frameLogPoints[5] - s_frameLogPoints[4] + (s_frameLogPoints[4] <= s_frameLogPoints[5] ? 0 : 5))

	);
#endif
}

//------------------------------------------------------------------------------
void Game::PreWorldInit()
{
	// Mostly just graphical resets
	m_sprites.ClearAllSprites();
	PAL_setColorsDMA(0, palette_black, 64);
	VDP_clearPlane(BG_A, true);
	VDP_clearPlane(BG_B, true);
	VDP_setHilightShadow(0);
}

}
