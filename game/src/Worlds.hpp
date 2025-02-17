#pragma once

#include "Declare.hpp"
#include "Constants.hpp"
#include "GameRoutines.hpp"

#include <array>

namespace Game
{

class World
{
public:
	virtual ~World() = default;

	virtual WorldRoutine Init(Game &io_game) = 0;
	virtual WorldRoutine Shutdown(Game &io_game) = 0;
	virtual void Run(Game &io_game) = 0;
};

}