#include "adventure-md/TitleWorld.hpp"

#include "adventure-md/Debug.hpp"
#include "adventure-md/Game.hpp"
#include "adventure-md/VNWorld.hpp"

namespace Game
{

//------------------------------------------------------------------------------
TitleWorld::TitleWorld
(
	std::unique_ptr<Script> i_script
)
	: m_script{ std::move(i_script) }
{
	Assert(m_script, "Must provide a script to TitleWorld");
}

//------------------------------------------------------------------------------
WorldRoutine TitleWorld::Init
(
	Game& io_game
)
{
	m_script->InitTitle(io_game, *this);
	
	while (io_game.TasksInProgress())
	{
		co_yield{};
	}

	co_return;
}

//------------------------------------------------------------------------------
WorldRoutine TitleWorld::Shutdown
(
	Game& io_game
)
{
	co_return;
}

//------------------------------------------------------------------------------
void TitleWorld::Run
(
	Game& io_game
)
{
	m_script->UpdateTitle(io_game, *this);
}

//------------------------------------------------------------------------------
void TitleWorld::GoToVNWorld
(
	Game& io_game
)
{
	io_game.RequestNextWorld(std::make_unique<VNWorld>(std::move(m_script)));
}

}