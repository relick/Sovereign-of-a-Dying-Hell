#pragma once

#include "Coroutine.hpp"

namespace Game
{

using DMARoutine = Coroutine<struct DMARoutineTag>;

using WorldRoutine = Coroutine<struct WorldRoutineCoroTag>;

using SceneRoutine = Coroutine<struct SceneRoutineTag>;

}