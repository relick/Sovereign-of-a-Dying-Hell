#pragma once

#include "Declare.hpp"
#include "Script.hpp"
#include "Worlds.hpp"

#include <memory>

namespace Game
{

class TitleWorld
	: public World
{
	std::unique_ptr<Script> m_script;

public:
	TitleWorld(std::unique_ptr<Script> i_script);

	WorldRoutine Init(Game& io_game) override;
	WorldRoutine Shutdown(Game& io_game) override;
	void Run(Game& io_game) override;
};

}