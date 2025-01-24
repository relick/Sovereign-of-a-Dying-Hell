#pragma once

#include "Coroutine.hpp"

namespace Game
{

using DMARoutine = Coroutine<struct DMARoutineTag, std::suspend_never>;//std::suspend_always>;

using WorldRoutine = Coroutine<struct WorldRoutineCoroTag, std::suspend_always>;

using SceneRoutine = Coroutine<struct SceneRoutineTag, std::suspend_never>;

}