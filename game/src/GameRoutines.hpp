#pragma once

#include "Coroutine.hpp"

namespace Game
{

using Task = Coroutine<struct TaskTag, std::suspend_always>;
enum class TaskPriority : u8
{
	Low,
	Normal,
	High,
	Animations,
};
#define AwaitTask(TASK_VAR) while(TASK_VAR) { TASK_VAR(); if (TASK_VAR) { co_yield{}; } }

using WorldRoutine = Coroutine<struct WorldRoutineCoroTag, std::suspend_always>;

using SceneRoutine = Coroutine<struct SceneRoutineTag, std::suspend_never>;

}