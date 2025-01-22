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

//------------------------------------------------------------------------------
void Game::Run()
{
	while (true)
	{
		if (!m_nextWorld)
		{
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
					m_currentWorldRoutine();
					PostWorldFrame();
				}
			}

			PreWorldInit();

			m_currentWorldRoutine = m_nextWorld->Init(*this);
			while (m_currentWorldRoutine)
			{
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
	m_sprites.Update();
	SYS_doVBlankProcess();
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
