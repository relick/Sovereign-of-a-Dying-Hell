#pragma once

#include "Coroutine.hpp"

namespace Game
{

using Task = Coroutine<struct TaskTag, std::suspend_always>;

using WorldRoutine = Coroutine<struct WorldRoutineCoroTag, std::suspend_always>;

using SceneRoutine = Coroutine<struct SceneRoutineTag, std::suspend_never>;

}