#include "Game.hpp"

#include "Worlds.hpp"

#include <genesis.h>

namespace Game
{

Game::Game
(
)
{
	VDP_setScreenWidth320();
	VDP_setScreenHeight224();
}

void Game::RequestNextWorld(std::unique_ptr<World> &&i_nextWorld)
{
	m_nextWorld = std::move(i_nextWorld);
}

void Game::Run()
{
	while (true)
	{
		if (m_nextWorld)
		{
			if (m_curWorld)
			{
				m_curWorld->Shutdown(*this);
			}
			m_nextWorld->Init(*this);
			std::swap(m_curWorld, m_nextWorld);
			m_nextWorld = nullptr;
		}
		m_curWorld->Run(*this);
		SYS_doVBlankProcess();
	}
}

}