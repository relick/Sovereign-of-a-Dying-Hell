#include "Game.hpp"

#include "Worlds.hpp"

#include <array>

namespace Game
{

#define LOG_WHOLE_FRAME_TIMES (1 && PROFILER)
#define LOG_MEMORY_USAGE (0)

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

	// The 'hard' limit is just over 7kB for NTSC
	// By being substantially lower than that we allow some room for extra
	// CPU processing and for dialogue text DMA to happen in parallel
	DMA_setMaxTransferSize(4096);
	DMA_setIgnoreOverCapacity(true);

	PreWorldInit();
}

//------------------------------------------------------------------------------
void Game::RequestNextWorld(std::unique_ptr<World> i_nextWorld)
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
	m_tasks.push_back({ std::move(i_task), {}, i_priority, s_taskMonotonic++, });
	std::push_heap(m_tasks.begin(), m_tasks.end());
}

//------------------------------------------------------------------------------
bool Game::TasksInProgress() const
{
	return DMA_getQueueSize() > 0 || !m_tasks.empty();
}

static constexpr std::array<u16, 4> c_magicString = {
	0x27FE,
	0x7580,
	0xC823,
	0x5F83,
};

//------------------------------------------------------------------------------
void Game::SaveVariables()
{
	SRAM_enable();

	auto fnWriteWord = [offset = u32{ 0 }] (u16 i_val) mutable
	{
		SRAM_writeWord(offset, i_val);
		offset += 2;
	};

	// TODO: Save data twice, with a magic string and version number with each.
	// this will help avoid issues on actual cartridges
	for (u16 const magic : c_magicString)
	{
		fnWriteWord(magic);
	}

	fnWriteWord(m_gameVariables.size());
	for (u16 var : m_gameVariables)
	{
		fnWriteWord(var);
	}

	SRAM_disable();
}

//------------------------------------------------------------------------------
bool Game::LoadVariables()
{
	SRAM_enableRO();

	auto fnReadWord = [offset = u32{ 0 }] mutable -> u16
	{
		u16 val = SRAM_readWord(offset);
		offset += 2;
		return val;
	};

	bool needsClearing = false;
	for (u16 const magic : c_magicString)
	{
		u16 const savedMagic = fnReadWord();
		if (magic != savedMagic)
		{
			needsClearing = true;
			break;

		}
	}

	if (needsClearing)
	{
		SRAM_enable();

		// Need to clear SRAM
		u32 const sramSize = (rom_header.sram_end - rom_header.sram_start) / 2;
		for (u32 i = 0; i < sramSize; ++i)
		{
			SRAM_writeByte(i, 0);
		}

		SRAM_disable();

		m_loadedData = false;
		return m_loadedData;
	}

	u16 const varCount = fnReadWord();
	m_gameVariables.resize(varCount);
	for (u16 i = 0; i < varCount; ++i)
	{
		m_gameVariables[i] = fnReadWord();
	}

	SRAM_disable();

	m_loadedData = varCount > 0;
	return m_loadedData;
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

#if LOG_MEMORY_USAGE
	kprintf("Used: %u, Free: %u", MEM_getAllocated(), MEM_getFree());
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
