#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/Script.hpp"
#include "adventure-md/Worlds.hpp"

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

	void GoToVNWorld(Game& io_game);
};

}