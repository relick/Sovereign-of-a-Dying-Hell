#pragma once

#include "Declare.hpp"

#include <memory>

namespace Game
{

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;

public:
	void RequestNextWorld(std::unique_ptr<World> &&i_nextWorld);

	void Run();
};

}