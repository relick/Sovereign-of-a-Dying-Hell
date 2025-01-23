#include "Game.hpp"

#include "Worlds.hpp"

#include <genesis.h>
#include <array>

namespace Game
{

#if PROFILER
static u32 volatile s_frameLineCount{};
#endif

#if LOG_WHOLE_FRAME_TIMES
static std::array<std::pair<u16, u16>, 4> fps{};
#endif

//------------------------------------------------------------------------------
Game::Game()
{
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
			fps[0] = {s_frameLineCount, GET_VCOUNTER};
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
					fps[0] = {s_frameLineCount, GET_VCOUNTER};
#endif
					m_currentWorldRoutine();
					PostWorldFrame();
				}
			}

			PreWorldInit();

			m_currentWorldRoutine = m_nextWorld->Init(*this);
			while (m_currentWorldRoutine)
			{
#if LOG_WHOLE_FRAME_TIMES
				fps[0] = {s_frameLineCount, GET_VCOUNTER};
#endif
				m_currentWorldRoutine();
				PostWorldFrame();
			}

			std::swap(m_curWorld, m_nextWorld);
			m_nextWorld = nullptr;
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
void Game::VIntCallback()
{
#if PROFILER
	s_frameLineCount += 261;
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
	fps[1] = {s_frameLineCount, GET_VCOUNTER};
#endif

	m_sprites.Update();

#if LOG_WHOLE_FRAME_TIMES
	fps[2] = {s_frameLineCount, GET_VCOUNTER};
#endif

	SYS_doVBlankProcess();

#if LOG_WHOLE_FRAME_TIMES
	fps[3] = {s_frameLineCount, GET_VCOUNTER};

	kprintf("VCounts: (%x, %x), (%x, %x), (%x, %x), (%x, %x).\nFrame time: %u. Sprites time: %u. VBlank time (wait/actual): %x, %u",
		fps[0].first, fps[0].second, fps[1].first, fps[1].second, 
		fps[2].first, fps[2].second, fps[3].first, fps[3].second,
		fps[1].second - fps[0].second + ((fps[1].first - fps[0].first) * 261) + (fps[0].second <= fps[1].second ? 0 : 261),
		fps[2].second - fps[1].second + ((fps[2].first - fps[1].first) * 261) + (fps[1].second <= fps[2].second ? 0 : 261),
		fps[3].second - fps[2].second + ((fps[3].first - (fps[2].first + 1)) * 261) + (fps[2].second <= fps[3].second ? 0 : 261),
		fps[3].second - 229 + (229 <= fps[3].second ? 0 : 256) + ((fps[3].first - (fps[2].first + 1)) * 261)
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
