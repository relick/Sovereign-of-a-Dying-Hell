#pragma once

#include "Coroutine.hpp"

namespace Game
{

using WorldRoutine = Coroutine<struct WorldRoutineCoroTag>;

using SceneRoutine = Coroutine<struct SceneRoutineTag>;

}