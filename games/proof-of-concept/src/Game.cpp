#include "Game.hpp"

#include "Worlds.hpp"

#include <genesis.h>

namespace Game
{

//------------------------------------------------------------------------------
Game::Game()
{
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

#define CHECK_FPS 1
#if CHECK_FPS
static u16 fps[4] = {0};
#endif

//------------------------------------------------------------------------------
void Game::Run()
{
	while (true)
	{
		if (!m_nextWorld)
		{
#if CHECK_FPS
			fps[0] = GET_VCOUNTER;
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
#if CHECK_FPS
					fps[0] = GET_VCOUNTER;
#endif
					m_currentWorldRoutine();
					PostWorldFrame();
				}
			}

			PreWorldInit();

			m_currentWorldRoutine = m_nextWorld->Init(*this);
			while (m_currentWorldRoutine)
			{
#if CHECK_FPS
				fps[0] = GET_VCOUNTER;
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
#if CHECK_FPS
	fps[1] = GET_VCOUNTER;
#endif
	m_sprites.Update();
#if CHECK_FPS
	fps[2] = GET_VCOUNTER;
#endif
	SYS_doVBlankProcess();
#if CHECK_FPS
	fps[3] = GET_VCOUNTER;

	kprintf("VCounts: %u, %u, %u, %u. Frame time: %u. Sprites time: %u. VBlank time (wait/actual): %u, %u",
		fps[0], fps[1], fps[2], fps[3],
		fps[1] - fps[0] + (fps[0] <= fps[1] ? 0 : 261),
		fps[2] - fps[1] + (fps[1] <= fps[2] ? 0 : 261),
		fps[3] - fps[2] + (fps[2] <= fps[3] ? 0 : 261),
		fps[3] - 229 + (229 <= fps[3] ? 0 : 256)
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
